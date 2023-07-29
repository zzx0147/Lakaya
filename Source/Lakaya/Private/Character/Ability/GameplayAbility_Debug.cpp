// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/GameplayAbility_Debug.h"

void UGameplayAbility_Debug::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	AddDebugMessage(TEXT("Abillity Activated"), ActivationInfo);
}

UGameplayAbility_Debug::UGameplayAbility_Debug()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGameplayAbility_Debug::InputPressed(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	AddDebugMessage(TEXT("Input Pressed"), ActivationInfo);
}

void UGameplayAbility_Debug::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	AddDebugMessage(TEXT("Input Released"), ActivationInfo);
	if (bEndOnRelease) EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGameplayAbility_Debug::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo,
                                        bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	AddDebugMessage(TEXT("Ability Ended"), ActivationInfo);
}

void UGameplayAbility_Debug::CancelAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	AddDebugMessage(TEXT("Ability Cancelled"), ActivationInfo);
}
