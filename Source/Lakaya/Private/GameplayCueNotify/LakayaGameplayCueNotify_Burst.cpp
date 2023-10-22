// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayCueNotify/LakayaGameplayCueNotify_Burst.h"
#include "GameplayCueNotifyTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraLensEffectInterface.h"
#include "Engine/World.h"
#include "GameFramework/ForceFeedbackEffect.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "Camera/CameraShakeBase.h"
#include "Components/ForceFeedbackComponent.h"
#include "Materials/MaterialInterface.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "GameFramework/InputDeviceProperties.h"
#include "Sound/SoundWaveProcedural.h"
bool ULakayaGameplayCueNotify_Burst::OnExecute_Implementation(AActor* Target,
                                                              const FGameplayCueParameters& Parameters) const
{
	UWorld* World = (Target ? Target->GetWorld() : GetWorld());
	
	FGameplayCueNotify_SpawnContext SpawnContext(World, Target, Parameters);
	//
	// if(DefaultPlacementInfo.bOverrideRotation)
	// {
	// 	OverridePlacementInfo = DefaultPlacementInfo;
	// 	
	// 	OverridePlacementInfo.RotationOverride = SpawnContext.HitResult->ImpactNormal.Rotation().Add(DefaultPlacementInfo.RotationOverride.Pitch,DefaultPlacementInfo.RotationOverride.Yaw,DefaultPlacementInfo.RotationOverride.Roll);
	// }
	// SpawnContext.SetDefaultSpawnCondition(&DefaultSpawnCondition);
	// SpawnContext.SetDefaultPlacementInfo(&DefaultPlacementInfo);
	//
	//
	// if (DefaultSpawnCondition.ShouldSpawn(SpawnContext))
	// {
	// 	FGameplayCueNotify_SpawnResult SpawnResult;
	//
	// 	BurstEffects.ExecuteEffects(SpawnContext, SpawnResult);
	// 	
	// 	OnBurst(Target, Parameters, SpawnResult);
	// }

	return false;
}
