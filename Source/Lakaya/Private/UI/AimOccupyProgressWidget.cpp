// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AimOccupyProgressWidget.h"
#include "Character/LakayaBasePlayerState.h"

void UAimOccupyProgressWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Percent = 0.0;

	if (AimOccupyProgressbar == nullptr) UE_LOG(LogTemp, Warning, TEXT("NULL"));

	if (IngTextImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("IngTextImage is null."));
	if (FinishTextImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("FinishTextImage is null."));
	if (CrashTextImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("CrashTextImage is null."));
}

void UAimOccupyProgressWidget::SetAimOccupyProgressBar(const ETeam& NewPlayerTeam, const float& NewProgress, const bool& bIsNewOccupy)
{
	if (!IsValid(AimOccupyProgressbar) || !IsValid(CrashTextImage) || !IsValid(IngTextImage))
	{
		UE_LOG(LogTemp, Warning, TEXT("AimOccupyProgressbar is null. || CrashTextImage is null. || IngTextImage is null."));
		return;
	}
	
	if (CurrentTeam != ETeam::None)
	{
		Percent = NewProgress;
	
		if (bIsNewOccupy)
		{
			AimOccupyProgressbar->SetPercent(Percent / 4);
			CrashTextImage->SetVisibility(ESlateVisibility::Hidden);
			IngTextImage->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			AimOccupyProgressbar->SetPercent(0);
			IngTextImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentTeam is None."));
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

void UAimOccupyProgressWidget::InitAimOccupyWidget() const
{
	if (!IsValid(IngTextImage) || !IsValid(CrashTextImage) || !IsValid(FinishTextImage))
	{
		UE_LOG(LogTemp, Warning, TEXT("IngTextImage or CrashTextImage or FinishTextImage is null."));
		return;
	}
	
	IngTextImage->SetVisibility(ESlateVisibility::Hidden);
	CrashTextImage->SetVisibility(ESlateVisibility::Hidden);
	FinishTextImage->SetVisibility(ESlateVisibility::Hidden);
}

void UAimOccupyProgressWidget::OccupySuccess()
{
	if (!IsValid(FinishTextImage)) return;
	
	FinishTextImage->SetVisibility(ESlateVisibility::Visible);
	
	FTimerDelegate TDelegate_FinishText;
	TDelegate_FinishText.BindLambda([this]
	{
		if (this == nullptr) return;
		InitAimOccupyWidget();
	});
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_FinishText, TDelegate_FinishText, 1.5f, false);
}

void UAimOccupyProgressWidget::SetCurrentTeam(const ETeam& NewTeam)
{
	if (!IsValid(AimOccupyProgressbar))
	{
		UE_LOG(LogTemp, Warning, TEXT("AimOccupyProgressbar is null."));
		return;
	}
	
	if (NewTeam == ETeam::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("NewTeam is none."));
		return;
	}
	
	CurrentTeam = NewTeam;
	
	FSlateBrush NewBrush;
	
	if (CurrentTeam == ETeam::Anti)
	{
		NewBrush.SetResourceObject(GetOccupyChargeAntiTexture());
	}
	else if (CurrentTeam == ETeam::Pro)
	{
		NewBrush.SetResourceObject(GetOccupyChargeProTexture());
	}
	
	AimOccupyProgressbar->WidgetStyle.SetFillImage(NewBrush);
}