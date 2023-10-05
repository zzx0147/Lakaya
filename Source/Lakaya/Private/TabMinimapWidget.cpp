// Fill out your copyright notice in the Description page of Project Settings.


#include "TabMinimapWidget.h"

#include "Components/CanvasPanelSlot.h"

void UTabMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MinimapImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("MinimapImage is null."));
	
	UpdateMinimap = false;
	IconAlignment = FVector2D(0.5f, 0.5f);
	IconSize = FVector2D(12.0f, 12.0f);
}

void UTabMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FVector2D UTabMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
	const FVector2D& MiniMapSize)
{
	// TODO : 맵의 전체 크기를 가져오는 함수가 있을까...?
	// WorldMap Size.
	const FVector2D WorldMapSize(7560.0f, 10600.0f);
	
	const float XRatio = (PlayerLocation.X) / (WorldMapSize.X / MiniMapSize.X);
	const float YRatio = (PlayerLocation.Y) / (WorldMapSize.Y / MiniMapSize.Y);
	
	const FVector2D MiniMapCoordinates(XRatio, YRatio);
	
	return MiniMapCoordinates;
}

void UTabMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	if (!MinimapImage) return;
}