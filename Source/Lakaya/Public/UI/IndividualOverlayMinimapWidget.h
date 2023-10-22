// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/OverlayMinimapWidget.h"
#include "IndividualOverlayMinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UIndividualOverlayMinimapWidget : public UOverlayMinimapWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
