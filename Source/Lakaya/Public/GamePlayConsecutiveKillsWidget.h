// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GamePlayConsecutiveKillsWidget.generated.h"

/**
 *
 */

class UProgressBar;

UCLASS()
class LAKAYA_API UGamePlayConsecutiveKillsWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UGamePlayConsecutiveKillsWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	void OnChangeConsecutiveKills(int NewConsecutiveKills);

private:
	UProgressBar* ConsecutiveKillsProgressBar;
	int16 ConsecutiveKills;
	int16 MaximumConsecutiveKills;
};
