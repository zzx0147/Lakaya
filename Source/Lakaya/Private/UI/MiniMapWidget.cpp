// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MiniMapWidget.h"

#include "Character/LakayaBaseCharacter.h"

void UMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateMinimap = false;
	
	IconAlignment = FVector2D(0.5f, 0.5f);
	IconSize = FVector2D(62.0f, 112.0f);
	
	OccupationPlayersByMinimap.Emplace(ETeam::Anti);
	OccupationPlayersByMinimap.Emplace(ETeam::Pro);
}

FVector2D UMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
                                                           const FVector2D& MiniMapSize)
{
	// WorldMap Size.
	const FVector2D WorldMapSize(7560.0f, 10600.0f);
	
	const float XRatio = (PlayerLocation.X) / (WorldMapSize.X / MiniMapSize.X);
	const float YRatio = (PlayerLocation.Y) / (WorldMapSize.Y / MiniMapSize.Y);
	
	const FVector2D MiniMapCoordinates(XRatio, YRatio);
	
	return MiniMapCoordinates;
}