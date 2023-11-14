// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OccupyExpressElementWidget.h"

void UOccupyExpressElementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ProgressBar->SetPercent(0);
}

void UOccupyExpressElementWidget::InitOccupyExpressElementWidget(const uint8& CaptureId) const
{
	switch (CaptureId)
	{
	case 1:
		InImage->SetBrushFromTexture(AntiAreaNoneImage);
		break;
	case 2:
		InImage->SetBrushFromTexture(CenterAreaNoneImage);
		break;
	case 3:
		InImage->SetBrushFromTexture(ProAreaNoneImage);
		break;
	default:
		break;
	}

	ProgressBar->SetPercent(0);
}