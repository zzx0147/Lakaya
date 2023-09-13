// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_GunFire.h"

#include "AbilitySystemComponent.h"
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

FGameplayAbilityTargetDataHandle ULakayaAbility_GunFire::MakeTargetData_Implementation()
{
	const auto TargetDataHandle = FireTrace();
	BP_ApplyGameplayEffectToTarget(TargetDataHandle, DamageEffect);
	return TargetDataHandle;
}

void ULakayaAbility_GunFire::OnTargetDataReceived_Implementation(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag GameplayTag)
{
	Super::OnTargetDataReceived_Implementation(TargetDataHandle, GameplayTag);

	// 클라이언트를 신뢰하기로 하는 경우 그냥 TargetDataHandle을 그대로 사용합니다.
	const auto FixedTargetDataHandle = bTrustClientHitResult ? TargetDataHandle : SimulateFireTrace(TargetDataHandle);
	ConsumeTargetData();
	BP_ApplyGameplayEffectToTarget(FixedTargetDataHandle, DamageEffect);
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
		ensure(TargetDataHandle.IsValid(0) && FireInterface))
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
