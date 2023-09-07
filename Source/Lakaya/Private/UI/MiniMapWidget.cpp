// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MiniMapWidget.h"

void UMiniMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MiniMap_Image = Cast<UImage>(GetWidgetFromName(TEXT("MiniMapImage")));

	if (MiniMap_Image.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("MiniMap_Image is Initialized."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MiniMap_Image is Null."));
	}
}