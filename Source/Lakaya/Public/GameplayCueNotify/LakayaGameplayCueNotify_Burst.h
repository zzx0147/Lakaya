// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Burst.h"
#include "LakayaGameplayCueNotify_Burst.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API ULakayaGameplayCueNotify_Burst : public UGameplayCueNotify_Burst
{
	GENERATED_BODY()

protected:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const override;
	

protected:
	// mutable FGameplayCueNotify_PlacementInfo OverridePlacementInfo;

	//DefaultPlacementInfo의 RotationOverride값을 이펙트 스폰 로테이션에 추가해주는 기능입니다, 작동하려면 DefaultPlacementInfo.bOverrideRotation가 켜져있어야 하고, 각 이펙트들의 RotationOverried가 꺼져있어야 합니다
	UPROPERTY(EditDefaultsOnly)
	uint8 bUseDefaultPlacementInfoToAdditiveRotation : 1;
};
