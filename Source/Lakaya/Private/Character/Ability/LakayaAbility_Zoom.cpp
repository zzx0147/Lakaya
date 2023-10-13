// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_Zoom.h"

void ULakayaAbility_Zoom::InputReleased(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

ULakayaAbility_Zoom::ULakayaAbility_Zoom(): Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	ZoomFov = 60.0f;
}

void ULakayaAbility_Zoom::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	SetZoom(true, ZoomFov, ActorInfo);
}

void ULakayaAbility_Zoom::NativeEndAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::NativeEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	SetZoom(false, ZoomFov, ActorInfo);
}
