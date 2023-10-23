// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TabMinimapWidget.h"
#include "IndividualTabMinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UIndividualTabMinimapWidget : public UTabMinimapWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	// Onwer의 PlayerState입니다.
	UPROPERTY()
	TObjectPtr<ALakayaBasePlayerState> PlayerState;
};