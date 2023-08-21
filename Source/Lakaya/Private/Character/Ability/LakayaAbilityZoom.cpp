// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbilityZoom.h"

#include "ETC/LakayaPlayerCameraManager.h"

ULakayaAbilityZoom::ULakayaAbilityZoom(): Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void ULakayaAbilityZoom::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	Log(ActorInfo, TEXT("OnGiveAbility Zoom!"));
}

void ULakayaAbilityZoom::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	Log(ActorInfo, TEXT("Activate Zoom Ability"));
	if (ActorInfo->PlayerController.IsValid())
	{
		if(ALakayaPlayerCameraManager* LakayaPlayerCameraManager = Cast<ALakayaPlayerCameraManager>(ActorInfo->PlayerController->PlayerCameraManager))
		{
			LakayaPlayerCameraManager->Zoom(true);
		}
	}
}

void ULakayaAbilityZoom::NativeEndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::NativeEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	Log(ActorInfo, TEXT("End Zoom Ability"));

	if (ActorInfo->PlayerController.IsValid())
	{
		if(ALakayaPlayerCameraManager* LakayaPlayerCameraManager = Cast<ALakayaPlayerCameraManager>(ActorInfo->PlayerController->PlayerCameraManager))
		{
			LakayaPlayerCameraManager->Zoom(false);
		}
	}
}

void ULakayaAbilityZoom::NativeCancelAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             bool bReplicateCancelAbility)
{
	Super::NativeCancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}
