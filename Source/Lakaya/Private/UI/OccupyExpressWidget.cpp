// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OccupyExpressWidget.h"

void UOccupyExpressWidget::SetAimOccupyProgressBar(const float NewProgress, const bool bIsNewOccupy)
{
	Percent = NewProgress;
	
	if (bIsNewOccupy)
	{
		if (AimOccupyProgressBar->GetVisibility() == ESlateVisibility::Hidden)
			AimOccupyProgressBar->SetVisibility(ESlateVisibility::Visible);
		AimOccupyProgressBar->SetPercent(Percent / 4);
	}
	else
	{
		if (AimOccupyProgressBar->GetVisibility() == ESlateVisibility::Visible)
			AimOccupyProgressBar->SetVisibility(ESlateVisibility::Hidden);
		AimOccupyProgressBar->SetPercent(Percent / 4);
	}
}
