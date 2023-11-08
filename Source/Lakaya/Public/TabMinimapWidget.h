// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MiniMapWidget.h"
#include "TabMinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UTabMinimapWidget : public UMinimapWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
};