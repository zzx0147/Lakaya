// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "LakayaPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API ALakayaPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	void Zoom(const bool IsZoomIn);
};
