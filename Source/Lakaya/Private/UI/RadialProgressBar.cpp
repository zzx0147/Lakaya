// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RadialProgressBar.h"

#include "Components/Image.h"

void URadialProgressBar::SetPercent(const float& NewPercent)
{
	Percent = NewPercent;
	if (ProgressBarDynamicMaterial)
	{
		ProgressBarDynamicMaterial->SetScalarParameterValue(TEXT("Percent"), NewPercent);
	}
}

void URadialProgressBar::SetFillImage(const TObjectPtr<UTexture2D> NewImage)
{
	if (NewImage != nullptr)
	{
		// FillImage = NewImage;
		if (ProgressBarDynamicMaterial)
		{
			ProgressBarDynamicMaterial->SetTextureParameterValue(TEXT("FillImage"), NewImage);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NewImage is null."));
	}
}

void URadialProgressBar::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (const auto& BrushMaterial = Cast<UMaterialInterface>(RadialProgressBarImage->GetBrush().GetResourceObject()))
	{
		ProgressBarDynamicMaterial = UMaterialInstanceDynamic::Create(BrushMaterial, this);

		ProgressBarDynamicMaterial->SetTextureParameterValue(TEXT("EmptyImage"), EmptyImage);
		ProgressBarDynamicMaterial->SetTextureParameterValue(TEXT("FillImage"), FillImage);
		ProgressBarDynamicMaterial->SetScalarParameterValue(TEXT("Percent"), Percent);
		RadialProgressBarImage->SetBrushFromMaterial(ProgressBarDynamicMaterial);
	}
}
