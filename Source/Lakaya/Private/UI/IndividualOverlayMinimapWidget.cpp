// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IndividualOverlayMinimapWidget.h"

void UIndividualOverlayMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ParentPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("PlayerImagePanel")));

	
}

void UIndividualOverlayMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// for (const auto& Enemy :)
}

UImage* UIndividualOverlayMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
{
	return Super::CreatePlayerImage(NewTeam, bMyPlayer);
}

void UIndividualOverlayMinimapWidget::UpdatePlayerPosition(const ETeam& NewTeam,
	const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState)
{
	Super::UpdatePlayerPosition(NewTeam, NewPlayerState);
}

FVector2d UIndividualOverlayMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
	const FVector2D& MiniMapSize)
{
	return Super::ConvertWorldToMiniMapCoordinates(PlayerLocation, MiniMapSize);
}

// void UIndividualOverlayMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
// {
// }

void UIndividualOverlayMinimapWidget::SetEnemyImage() const
{
}

void UIndividualOverlayMinimapWidget::UpdateMinimapImagePositionAndRotation(
	const ALakayaBasePlayerState& NewPlayerState, const FVector2D NewPosition) const
{
	Super::UpdateMinimapImagePositionAndRotation(NewPlayerState, NewPosition);
}
