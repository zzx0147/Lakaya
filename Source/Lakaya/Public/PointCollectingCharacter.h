// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerCharacter.h"
#include "PointCollectingCharacter.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API APointCollectingCharacter : public ABasePlayerCharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Point)
	class UPointComponent* PointComponent;
	
public:
	APointCollectingCharacter();
};
