// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_Zoom.h"

#include "ETC/LakayaPlayerCameraManager.h"

ULakayaAbility_Zoom::ULakayaAbility_Zoom(): Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
	ZoomFov = 60.0f;
}

void ULakayaAbility_Zoom::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	Log(ActorInfo, TEXT("OnGiveAbility Zoom!"));
}

void ULakayaAbility_Zoom::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	Log(ActorInfo, TEXT("Activate Zoom Ability"));
	if (ActorInfo->PlayerController.IsValid())
	{
		if(ALakayaPlayerCameraManager* LakayaPlayerCameraManager = Cast<ALakayaPlayerCameraManager>(ActorInfo->PlayerController->PlayerCameraManager))
		{
			LakayaPlayerCameraManager->Zoom(true, ZoomFov);
		}
	}
}

void ULakayaAbility_Zoom::NativeEndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::NativeEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	Log(ActorInfo, TEXT("End Zoom Ability"));

	if (ActorInfo->PlayerController.IsValid())
	{
		if(ALakayaPlayerCameraManager* LakayaPlayerCameraManager = Cast<ALakayaPlayerCameraManager>(ActorInfo->PlayerController->PlayerCameraManager))
		{
			LakayaPlayerCameraManager->Zoom(false, ZoomFov);
		}
	}
}

void ULakayaAbility_Zoom::NativeCancelAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             bool bReplicateCancelAbility)
{
	Super::NativeCancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
