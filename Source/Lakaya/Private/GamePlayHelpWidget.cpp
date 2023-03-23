// Fill out your copyright notice in the Description page of Project Settings.

#define DO_CHECK 1

#include "GamePlayHelpWidget.h"
#include "Components/CanvasPanel.h"

UGamePlayHelpWidget::UGamePlayHelpWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
}

void UGamePlayHelpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HelpBodyPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("HelpBodyPanel")));
	check(HelpBodyPanel != nullptr);
}

void UGamePlayHelpWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry,InDeltaTime);
}
