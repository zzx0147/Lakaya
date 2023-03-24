// Fill out your copyright notice in the Description page of Project Settings.

#define DO_CHECK 1

#include "GamePlayCrosshairWidget.h"
#include "Components/Image.h"

UGamePlayCrosshairWidget::UGamePlayCrosshairWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UGamePlayCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 후 널체크
#pragma region InitAndNullCheck

	CrosshairImage = Cast<UImage>(GetWidgetFromName(TEXT("Crosshair_Img")));
	check(CrosshairImage != nullptr);

#pragma endregion

}

void UGamePlayCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
