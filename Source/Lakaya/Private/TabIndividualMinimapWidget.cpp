// Fill out your copyright notice in the Description page of Project Settings.


#include "TabIndividualMinimapWidget.h"

// TODO: 헤더파일에 선언된 기능을 구현해야 합니다.
void UTabIndividualMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTabIndividualMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FVector2D UTabIndividualMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
	const FVector2D& MiniMapSize)
{
	return Super::ConvertWorldToMiniMapCoordinates(PlayerLocation, MiniMapSize);
}

void UTabIndividualMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	Super::UpdatePlayerPosition(Team);
}

UImage* UTabIndividualMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
{
	return Super::CreatePlayerImage(NewTeam, bMyPlayer);
}
