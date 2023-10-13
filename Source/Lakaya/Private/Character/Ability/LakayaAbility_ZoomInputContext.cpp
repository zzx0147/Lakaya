// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_ZoomInputContext.h"

#include "Input/LakayaInputContext.h"

void ULakayaAbility_ZoomInputContext::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo,
                                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	AddMappingContext(ActorInfo, InputContext.LoadSynchronous());
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void ULakayaAbility_ZoomInputContext::NativeEndAbility(const FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* ActorInfo,
                                                       const FGameplayAbilityActivationInfo ActivationInfo,
                                                       bool bReplicateEndAbility, bool bWasCancelled)
{
	RemoveMappingContext(ActorInfo, InputContext.LoadSynchronous());
	Super::NativeEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
