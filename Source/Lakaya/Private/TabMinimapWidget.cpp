// Fill out your copyright notice in the Description page of Project Settings.


#include "TabMinimapWidget.h"

void UTabMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ParentPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("PlayerImagePanel")));
	
	MinimapSize = FVector2D(312.5f, 476.25f);
	WidgetOffset = FVector2D(960.0f, 545.5f);
}