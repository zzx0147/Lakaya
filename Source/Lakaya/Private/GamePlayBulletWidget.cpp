// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlayBulletWidget.h"

UGamePlayBulletWidget::UGamePlayBulletWidget(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
}

void UGamePlayBulletWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGamePlayBulletWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
