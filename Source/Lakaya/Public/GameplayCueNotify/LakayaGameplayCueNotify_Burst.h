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
};
