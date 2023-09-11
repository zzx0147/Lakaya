// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_InputContext.h"

#include "Input/LakayaInputContext.h"

ULakayaAbility_InputContext::ULakayaAbility_InputContext()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void ULakayaAbility_InputContext::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayAbilityActivationInfo ActivationInfo,
                                                  const FGameplayEventData* TriggerEventData)
{
	AddMappingContext(ActorInfo);
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void ULakayaAbility_InputContext::NativeEndAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo,
                                                   const FGameplayAbilityActivationInfo ActivationInfo,
                                                   bool bReplicateEndAbility, bool bWasCancelled)
{
	RemoveMappingContext(ActorInfo);
	Super::NativeEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULakayaAbility_InputContext::AddMappingContext(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (InputContext.IsNull()) return;
	if (const auto InputSubsystem = GetEnhancedInputSubsystem(ActorInfo))
	{
		InputContext.LoadSynchronous()->AddMappingContext(InputSubsystem);
	}
}

void ULakayaAbility_InputContext::RemoveMappingContext(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (InputContext.IsNull()) return;
	if (const auto InputSubsystem = GetEnhancedInputSubsystem(ActorInfo))
	{
		InputContext.LoadSynchronous()->RemoveMappingContext(InputSubsystem);
	}
}
