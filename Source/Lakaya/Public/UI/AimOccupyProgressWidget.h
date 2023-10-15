// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "AimOccupyProgressWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UAimOccupyProgressWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	void SetAimOccupyProgressBar(const float& NewProgress, const bool& bIsNewOccupy);
	void Success();
	
private:
	// TODO : 후에 ProgresBar 에서 RadialSlider로 변경해야합니다.
	UPROPERTY()
	UProgressBar* AimOccupyChargeProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IngTextImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FinishTextImage;
	
	UPROPERTY()
	float Percent;

	FTimerHandle TimerHandle_FinishText;
};