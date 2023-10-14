// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OccupyExpressWidget.h"

void UOccupyExpressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Percent = 0;

	if (AimOccupyProgressBar == nullptr) UE_LOG(LogTemp, Warning, TEXT("AimOccupyProgressSlider is nullptr"));
}

void UOccupyExpressWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (AimOccupyProgressBar == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AimOccupyProgresBar is null."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Is Valid."));
	}
}

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
