// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "LakayaPlayerCameraManager.generated.h"

USTRUCT()
struct FMyStruct
{
	GENERATED_BODY()

	float e;

	float r;
};

/**
 * 
 */
UCLASS()
class LAKAYA_API ALakayaPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:


	UFUNCTION(BlueprintNativeEvent)
	void Zoom(const bool IsZoomIn, const float& ZoomFov = 0.0f);
	
	UFUNCTION(BlueprintCallable, DisplayName="SetFov")
	void K2_SetFov(const float& NewFov);

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly) float CrouchBlendDuration=0.2f;
	float CrouchBlendTime;
};
