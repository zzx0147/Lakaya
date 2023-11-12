// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Occupation/CaptureArea.h"
#include "CaptureAreaCapsule.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API ACaptureAreaCapsule : public ACaptureArea
{
	GENERATED_BODY()

public:
	ACaptureAreaCapsule();

private:
	UPROPERTY(VisibleAnywhere, Category = Capsule)
	TObjectPtr<class UCapsuleComponent> Trigger;
};