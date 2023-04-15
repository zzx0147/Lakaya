// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "LakayaCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API ALakayaCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	float CrouchBlendDuration=0.5f;
	
	float CrouchBlendTime;

	
public:
	ALakayaCameraManager();
	
	virtual void UpdateViewTarget(FTViewTarget& OutTV, float DeltaTime) override;
};
