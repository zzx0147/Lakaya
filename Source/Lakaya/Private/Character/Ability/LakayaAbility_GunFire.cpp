// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_GunFire.h"

#include "AbilitySystemComponent.h"
#include "GameplayCueFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/Ability/AbilityGunFireInterface.h"


ULakayaAbility_GunFire::ULakayaAbility_GunFire()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	bTrustClientHitResult = true;
}

void ULakayaAbility_GunFire::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (!ensureMsgf(InstancingPolicy != EGameplayAbilityInstancingPolicy::NonInstanced,
	                TEXT("사격 어빌리티는 반드시 인스턴싱되어야 합니다.")))
	{
		ActorInfo->AbilitySystemComponent->ClearAbility(Spec.Handle);
		return;
	}
}

void ULakayaAbility_GunFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	const auto bAuthority = ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Authority;

	// 리모트 서버에서는 클라이언트로부터 전달될 타겟 데이터를 기다립니다.
	if (bAuthority && !ActorInfo->IsLocallyControlled())
	{
		TargetDataDelegateHandle = GetTargetDataDelegate().AddUObject(this, &ThisClass::ServerOnTargetDataReady);
	}
	else
	{
		const auto TargetDataHandle = FireTrace();

		FScopedPredictionWindow PredictionWindow((GetAbilitySystemComponentFromActorInfo_Checked()));

		// 서버라면 그냥 곧바로 데미지 이펙트를 적용합니다.
		if (bAuthority)
		{
			BP_ApplyGameplayEffectToTarget(TargetDataHandle, DamageEffect);
		}
		// 클라이언트라면 타겟 데이터를 서버로 전송합니다.
		else
		{
			ServerSetReplicatedTargetData(TargetDataHandle);
		}

		ExecuteFireCue(TargetDataHandle);
		ActivateFireMontageTask();
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void ULakayaAbility_GunFire::NativeEndAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::NativeEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (TargetDataDelegateHandle.IsValid())
	{
		GetTargetDataDelegate().Remove(TargetDataDelegateHandle);
	}
}

void ULakayaAbility_GunFire::ServerOnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
                                                     FGameplayTag GameplayTag)
{
	// 클라이언트를 신뢰하기로 하는 경우 그냥 TargetDataHandle을 그대로 사용합니다.
	const auto FixedTargetDataHandle = bTrustClientHitResult ? TargetDataHandle : SimulateFireTrace(TargetDataHandle);
	ConsumeTargetData();

	ExecuteFireCue(FixedTargetDataHandle);
	BP_ApplyGameplayEffectToTarget(FixedTargetDataHandle, DamageEffect);
	ActivateFireMontageTask();
}

FGameplayAbilityTargetDataHandle ULakayaAbility_GunFire::FireTrace() const
{
	FGameplayAbilityTargetDataHandle TargetDataHandle;

	if (const auto FireInterface = FindGunFireInterface())
	{
		TArray<FHitResult> HitResults;
		FireInterface->FireTrace(HitResults);
		HitResultsToTargetDataHandle(HitResults, TargetDataHandle);
	}

	return TargetDataHandle;
}

FGameplayAbilityTargetDataHandle ULakayaAbility_GunFire::SimulateFireTrace(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle) const
{
	FGameplayAbilityTargetDataHandle ResultDataHandle;

	if (const auto FireInterface = FindGunFireInterface();
		ensure(TargetDataHandle.Num() > 0 && FireInterface))
	{
		TArray<FHitResult> HitResults;
		FireInterface->SimulateFireTrace(TargetDataHandle.Get(0)->GetOrigin(), HitResults);
		HitResultsToTargetDataHandle(HitResults, ResultDataHandle);
	}

	return ResultDataHandle;
}

IAbilityGunFireInterface* ULakayaAbility_GunFire::FindGunFireInterface() const
{
	auto FireInterface = Cast<IAbilityGunFireInterface>(GetAvatarActorFromActorInfo());
	if (!FireInterface)
	{
		FireInterface = Cast<IAbilityGunFireInterface>(GetOwningActorFromActorInfo());
	}

	ensureMsgf(FireInterface, TEXT("[%s] 아바타 액터 또는 오너 액터가 IAbilityGunFireInterface를 구현하지 않습니다."), *GetName());
	return FireInterface;
}

void ULakayaAbility_GunFire::HitResultsToTargetDataHandle(const TArray<FHitResult>& HitResults,
                                                          FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	static auto Transform = [](const FHitResult& HitResult)
	{
		return new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
	};
	Algo::Transform(HitResults, TargetDataHandle, Transform);
}

void ULakayaAbility_GunFire::OnMontageEnded()
{
	// 애니메이션 몽타주가 종료되었을 때 아직 어빌리티가 실행중인 경우 어빌리티를 종료합니다.
	if (IsActive())
	{
		K2_EndAbility();
	}
}

void ULakayaAbility_GunFire::ActivateFireMontageTask()
{
	const auto MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, FireMontage);
	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageEnded);
	MontageTask->ReadyForActivation();
}

void ULakayaAbility_GunFire::ExecuteFireCue(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	if (TargetDataHandle.Num() <= 0) return;
	K2_ExecuteGameplayCueWithParams(
		FireCueTag.GameplayCueTag,
		UGameplayCueFunctionLibrary::MakeGameplayCueParametersFromHitResult(*TargetDataHandle.Get(0)->GetHitResult()));
}

void ULakayaAbility_GunFire::ConsumeTargetData() const
{
	GetAbilitySystemComponentFromActorInfo_Checked()->ConsumeClientReplicatedTargetData(
		GetCurrentAbilitySpecHandle(), GetCurrentActivationInfo().GetActivationPredictionKey());
}
