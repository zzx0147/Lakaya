// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_GunFire.h"

#include "AbilitySystemComponent.h"
#include "Character/Ability/Component/AbilityComponent.h"
#include "Interface/FireTraceInterface.h"


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

	check(FireTraceComponentClass->ImplementsInterface(UFireTraceInterface::StaticClass()));

	TryRemoveAbilityComponent(ActorInfo->AvatarActor.Get());

	FireTraceComponent = FindOrAddAbilityComponent(ActorInfo->AvatarActor.Get(), FireTraceComponentClass,
	                                               bIsFireTraceComponentAdded);

	if (FireTraceComponent.IsValid())
	{
		BP_Log(FString::Printf(
			TEXT("FireTraceComponent %s : %s"), bIsFireTraceComponentAdded ? TEXT("Added") : TEXT("Found"),
			*FireTraceComponent->GetName()));
	}
	else
	{
		BP_Log(TEXT("Failed to add FireTraceComponent"));
	}
}

void ULakayaAbility_GunFire::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
	TryRemoveAbilityComponent(ActorInfo->AvatarActor.Get());
}

bool ULakayaAbility_GunFire::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                const FGameplayAbilityActorInfo* ActorInfo,
                                                const FGameplayTagContainer* SourceTags,
                                                const FGameplayTagContainer* TargetTags,
                                                FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)
		&& FireTraceComponent.IsValid();
}

FGameplayAbilityTargetDataHandle ULakayaAbility_GunFire::MakeTargetData_Implementation()
{
	check(FireTraceComponent.IsValid())

	TArray<FHitResult> HitResults;
	IFireTraceInterface::Execute_FireTrace(FireTraceComponent.Get(), HitResults);

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	HitResultsToTargetDataHandle(HitResults, TargetDataHandle);

	BP_ApplyGameplayEffectOrCueToTarget(TargetDataHandle, DamageEffect, EnvironmentCue.GameplayCueTag);
	return TargetDataHandle;
}

void ULakayaAbility_GunFire::OnTargetDataReceived_Implementation(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag GameplayTag)
{
	BP_ApplyGameplayEffectOrCueToTarget(TargetDataHandle, DamageEffect, EnvironmentCue.GameplayCueTag);
	Super::OnTargetDataReceived_Implementation(TargetDataHandle, GameplayTag);
}

void ULakayaAbility_GunFire::TryRemoveAbilityComponent(AActor* TargetActor)
{
	if (FireTraceComponent.IsValid() && bIsFireTraceComponentAdded && TargetActor)
	{
		TargetActor->RemoveInstanceComponent(FireTraceComponent.Get());
		FireTraceComponent.Reset();
		BP_Log(TEXT("FireTraceComponent Removed"));
	}
}
