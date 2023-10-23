// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayCueNotify/LakayaGameplayCueNotify_Burst.h"
#include "GameplayCueNotifyTypes.h"


bool ULakayaGameplayCueNotify_Burst::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	Super::OnExecute_Implementation(Target, Parameters);
	// UWorld* World = (Target ? Target->GetWorld() : GetWorld());
	//
	// FGameplayCueNotify_SpawnContext SpawnContext(World, Target, Parameters);
	//
	// if(DefaultPlacementInfo.bOverrideRotation && bUseDefaultPlacementInfoToAdditiveRotation)
	// {
	// 	OverridePlacementInfo = DefaultPlacementInfo;
	// 	
	// 	OverridePlacementInfo.RotationOverride = SpawnContext.HitResult->ImpactNormal.Rotation().Add(DefaultPlacementInfo.RotationOverride.Pitch,DefaultPlacementInfo.RotationOverride.Yaw,DefaultPlacementInfo.RotationOverride.Roll);
	// }
	// SpawnContext.SetDefaultSpawnCondition(&DefaultSpawnCondition);
	// SpawnContext.SetDefaultPlacementInfo(&OverridePlacementInfo);
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
