// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DynamicCrossHairWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UDynamicCrossHairWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintImplementableEvent)
	void OnNormalAttackHitEnemy();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnChangeBulletSpreadAmount(const float& NewAmount);
};