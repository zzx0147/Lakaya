// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

private:
	UPROPERTY()
	UProgressBar* AimOccupyChargeProgressBar;

	UPROPERTY()
	float Percent;
};