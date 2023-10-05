// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDMinimapWidget.h"

void UHUDMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHUDMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

UImage* UHUDMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
{
	return Super::CreatePlayerImage(NewTeam, bMyPlayer);
}

FVector2d UHUDMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
	const FVector2D& MiniMapSize)
{
	return Super::ConvertWorldToMiniMapCoordinates(PlayerLocation, MiniMapSize);
}

void UHUDMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	Super::UpdatePlayerPosition(Team);
}