// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameStateSequentialWidget.generated.h"

class UCommonActivatableWidgetSwitcher;
/**
 * 
 */
UCLASS()
class LAKAYA_API UGameStateSequentialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UCommonActivatableWidgetSwitcher* WidgetSwitcher;
};
