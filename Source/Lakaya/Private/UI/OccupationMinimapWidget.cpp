// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OccupationMinimapWidget.h"

#include "EngineUtils.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UOccupationMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MinimapImage = Cast<UImage>(GetWidgetFromName(TEXT("MinimapImage")));
	PlayerImage = Cast<UImage>(GetWidgetFromName(TEXT("PlayerImage")));

	UpdateMinimap = false;
}

void UOccupationMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (UpdateMinimap)
	{
		const auto PlayerActor = GetOwningPlayer()->GetPawn();
		if (PlayerActor)
		{
			const FVector2D PlayerLocation(PlayerActor->GetActorLocation().X, PlayerActor->GetActorLocation().Y);
			UpdatePlayerPosition(PlayerLocation);
		}
	}
}

void UOccupationMinimapWidget::UpdatePlayerPosition(const FVector2D& PlayerPosition)
{
	// MinimapResource Size
	const FVector2D MiniMapSize(250.0f, 381.0f);
	
	const FVector2D NewPlayerPosition = ConvertWorldToMiniMapCoordinates(PlayerPosition, MiniMapSize);

	// FVector2D = ((MinimapPosition.x + MinimapSize / 2), (MinimapPosition.y + MinimapSize / 2)
	PlayerImage->SetRenderTranslation(NewPlayerPosition + FVector2D(225.0f, 250.5f));
}

FVector2D UOccupationMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
	const FVector2D& MiniMapSize)
{
	// WorldMap Size.
	const FVector2D WorldMapSize(7560.0f, 10600.0f);
	
	const float XRatio = (PlayerLocation.X) / (WorldMapSize.X / MiniMapSize.X);
	const float YRatio = (PlayerLocation.Y) / (WorldMapSize.Y / MiniMapSize.Y);

	const FVector2D MiniMapCoordinates(XRatio, YRatio);

	return MiniMapCoordinates;
}