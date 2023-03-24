// Fill out your copyright notice in the Description page of Project Settings.

#define DO_CHECK 1

#include "UI/GamePlayWidget.h"

UGamePlayWidget::UGamePlayWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UGamePlayWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UGamePlayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}




