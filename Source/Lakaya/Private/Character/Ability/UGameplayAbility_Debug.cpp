// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/UGameplayAbility_Debug.h"

void UUGameplayAbility_Debug::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	AddDebugMessage(TEXT("Abillity Activated"), ActivationInfo);
}

void UUGameplayAbility_Debug::InputPressed(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	AddDebugMessage(TEXT("Input Pressed"), ActivationInfo);
}

void UUGameplayAbility_Debug::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	AddDebugMessage(TEXT("Input Released"), ActivationInfo);
	if (bEndOnRelease) EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UUGameplayAbility_Debug::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	AddDebugMessage(TEXT("Ability Ended"), ActivationInfo);
}

void UUGameplayAbility_Debug::CancelAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo,
                                            bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	AddDebugMessage(TEXT("Ability Cancelled"), ActivationInfo);
}
