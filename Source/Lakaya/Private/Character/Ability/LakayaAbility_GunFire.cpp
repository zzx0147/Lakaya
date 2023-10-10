// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_GunFire.h"

#include "AbilitySystemComponent.h"
#include "Character/Ability/Component/AbilityComponent_FireTrace.h"


ULakayaAbility_GunFire::ULakayaAbility_GunFire()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	bIsFireTraceComponentAdded = false;
}

void ULakayaAbility_GunFire::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	check(IsInstantiated())
	if (!ActorInfo->IsLocallyControlled())
	{
		return;
	}

	const auto TargetActor = ActorInfo->AvatarActor.Get();
	if (!TargetActor)
	{
		return;
	}

	FireTraceComponent = Cast<UAbilityComponent_FireTrace>(TargetActor->FindComponentByClass(FireTraceComponentClass));
	if (FireTraceComponent.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] FireTraceComponent found : %s"), *GetName(), *FireTraceComponent->GetName());
	}
	else
	{
		FireTraceComponent = Cast<UAbilityComponent_FireTrace>(
			TargetActor->AddComponentByClass(FireTraceComponentClass, false, FTransform::Identity, false));

		if (FireTraceComponent.IsValid())
		{
			bIsFireTraceComponentAdded = true;
			UE_LOG(LogTemp, Log, TEXT("[%s] FireTraceComponent added : %s"), *GetName(),
			       *FireTraceComponent->GetName());
		}
		else
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Spec.Handle);
			UE_LOG(LogTemp, Error, TEXT("[%s] Failed to add FireTraceComponent. Ability has been removed"), *GetName());
			return;
		}
	}

	FireTraceComponent->SetOwningAbility(this);
}

void ULakayaAbility_GunFire::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);

	if (bIsFireTraceComponentAdded)
	{
		if (const auto& TargetActor = ActorInfo->AvatarActor; TargetActor.IsValid())
		{
			TargetActor->RemoveInstanceComponent(FireTraceComponent.Get());
		}
	}
}

FGameplayAbilityTargetDataHandle ULakayaAbility_GunFire::MakeTargetData_Implementation()
{
	check(FireTraceComponent.IsValid())

	TArray<FHitResult> HitResults;
	FireTraceComponent->FireTrace(HitResults);

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	HitResultsToTargetDataHandle(HitResults, TargetDataHandle);

	BP_ApplyGameplayEffectOrCueToTarget(TargetDataHandle, DamageEffect, EnvironmentCue.GameplayCueTag);
	return TargetDataHandle;
}

void ULakayaAbility_GunFire::OnTargetDataReceived_Implementation(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag GameplayTag)
{
	Super::OnTargetDataReceived_Implementation(TargetDataHandle, GameplayTag);

	BP_ApplyGameplayEffectOrCueToTarget(TargetDataHandle, DamageEffect, EnvironmentCue.GameplayCueTag);
	ConsumeTargetData();
}
