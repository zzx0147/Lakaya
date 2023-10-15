// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AimOccupyProgressWidget.h"

void UAimOccupyProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Percent = 0.0;

	AimOccupyChargeProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("adad")));
	if (AimOccupyChargeProgressBar == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("NULL"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Valid."));
	}
}

void UAimOccupyProgressWidget::SetAimOccupyProgressBar(const float& NewProgress, const bool& bIsNewOccupy)
{
	if (AimOccupyChargeProgressBar)
	{
		Percent = NewProgress;
		
		if (bIsNewOccupy)
		{
			// if (AimOccupyChargeProgressBar->GetVisibility() == ESlateVisibility::Hidden)
				// AimOccupyChargeProgressBar->SetVisibility(ESlateVisibility::Visible);
			AimOccupyChargeProgressBar->SetPercent(Percent / 4);
			// AimOccupyProgressBar->SetValue(Percent / 4);
		}
		else
		{
			// if (AimOccupyChargeProgressBar->GetVisibility() == ESlateVisibility::Visible)
				// AimOccupyChargeProgressBar->SetVisibility(ESlateVisibility::Hidden);
			AimOccupyChargeProgressBar->SetPercent(0);
			// AimOccupyProgressBar->SetValue(Percent / 4);
		}
	}
}
