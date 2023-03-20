// Fill out your copyright notice in the Description page of Project Settings.
#define DO_CHECK 1

#include "GamePlayBulletWidget.h"
#include "Components/TextBlock.h"


UGamePlayBulletWidget::UGamePlayBulletWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
}

void UGamePlayBulletWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RemainBulletsText = Cast<UTextBlock>(GetWidgetFromName(TEXT("RemainBullets_Text")));
	MagazineCapacityText = Cast<UTextBlock>(GetWidgetFromName(TEXT("MagazineCapacity_Text")));

	check(RemainBulletsText);
	check(MagazineCapacityText);

}

void UGamePlayBulletWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGamePlayBulletWidget::OnChangeRemainBullets(int16 NewRemainBullets)
{
	RemainBullets = NewRemainBullets;
	RemainBulletsText->SetText(FText::AsNumber(NewRemainBullets));

}

void UGamePlayBulletWidget::OnChangeMagazineCapacity(int16 NewMagazineCapacity)
{
	MagazineCapacity = NewMagazineCapacity;
	MagazineCapacityText->SetText(FText::FromString(FString::Printf(TEXT("/%d"), NewMagazineCapacity)));
}
