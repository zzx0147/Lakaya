// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AimOccupyProgressWidget.h"

#include "Chaos/PBDEvolution.h"
#include "Character/LakayaBasePlayerState.h"

void UAimOccupyProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Percent = 0.0;

	AimOccupyChargeProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("adad")));
	
	if (AimOccupyChargeProgressBar == nullptr) UE_LOG(LogTemp, Warning, TEXT("NULL"));

	if (IngTextImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("IngTextImage is null."));
	if (FinishTextImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("FinishTextImage is null."));
	if (CrashTextImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("CrashTextImage is null."));
}

void UAimOccupyProgressWidget::SetAimOccupyProgressBar(const float& NewProgress, const bool& bIsNewOccupy)
{
	if (AimOccupyChargeProgressBar)
	{
		Percent = NewProgress;
	
		if (bIsNewOccupy)
		{
			AimOccupyChargeProgressBar->SetPercent(Percent / 4);
			CrashTextImage->SetVisibility(ESlateVisibility::Hidden);
			IngTextImage->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			AimOccupyChargeProgressBar->SetPercent(0);
			IngTextImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UAimOccupyProgressWidget::OccupyCrash() const
{
	if (IngTextImage == nullptr || CrashTextImage == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IngTextImage or CrashTextImage is null."));
		return;
	}

	if (IngTextImage->GetVisibility() == ESlateVisibility::Visible)
		IngTextImage->SetVisibility(ESlateVisibility::Hidden);

	CrashTextImage->SetVisibility(ESlateVisibility::Visible);
}

void UAimOccupyProgressWidget::AllAimWidgetDisable() const
{
	IngTextImage->SetVisibility(ESlateVisibility::Hidden);
	CrashTextImage->SetVisibility(ESlateVisibility::Hidden);
}

void UAimOccupyProgressWidget::Success()
{
	FinishTextImage->SetVisibility(ESlateVisibility::Visible);
	
	FTimerDelegate TDelegate_FinishText;
	TDelegate_FinishText.BindLambda([this]
	{
		if (this == nullptr) return;
		FinishTextImage->SetVisibility(ESlateVisibility::Hidden);
	});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_FinishText, TDelegate_FinishText, 1.5f, false);
}
