// Fill out your copyright notice in the Description page of Project Settings.


#include "IndividualTabMinimapWidget.h"

// TODO: 헤더파일에 선언된 기능을 구현해야 합니다.
void UIndividualTabMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UIndividualTabMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FVector2D UIndividualTabMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
	const FVector2D& MiniMapSize)
{
	return Super::ConvertWorldToMiniMapCoordinates(PlayerLocation, MiniMapSize);
}

void UIndividualTabMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	Super::UpdatePlayerPosition(Team);
}

UImage* UIndividualTabMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
{
	return Super::CreatePlayerImage(NewTeam, bMyPlayer);
}
