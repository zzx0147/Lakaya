// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Occupation/CaptureArea.h"
#include "CaptureAreaBox.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API ACaptureAreaBox : public ACaptureArea
{
	GENERATED_BODY()

public:
	ACaptureAreaBox();
	
private:
	UPROPERTY(VisibleAnywhere, Category = Box)
	TObjectPtr<class UBoxComponent> Trigger;
	
	UPROPERTY(EditAnywhere)
	FVector CaptureAreaRange;
};
