// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillStreakWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UKillStreakWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnChangeKillStreak(const int32& NewKillStreak);
	
};
