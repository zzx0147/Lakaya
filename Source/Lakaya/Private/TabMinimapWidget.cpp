// Fill out your copyright notice in the Description page of Project Settings.


#include "TabMinimapWidget.h"

void UTabMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MinimapImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("TabMinimapWidget_MinimapImage is null."));
}

void UTabMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FVector2D UTabMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
	const FVector2D& MiniMapSize)
{
	return Super::ConvertWorldToMiniMapCoordinates(PlayerLocation, MiniMapSize);
}

void UTabMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	Super::UpdatePlayerPosition(Team);
}

UImage* UTabMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
{
	return Super::CreatePlayerImage(NewTeam, bMyPlayer);
}
