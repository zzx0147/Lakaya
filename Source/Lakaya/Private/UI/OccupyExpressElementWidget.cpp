// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OccupyExpressElementWidget.h"

void UOccupyExpressElementWidget::InitOccupyExpressElementWidget(const uint8& CaptureId,
	const TObjectPtr<UTexture2D> NoneImage) const
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
}

void UOccupyExpressElementWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
}
