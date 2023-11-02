// Fill out your copyright notice in the Description page of Project Settings.


#include "TabMinimapWidget.h"

void UTabMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ParentPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("PlayerImagePanel")));
	
	MinimapSize = FVector2D(312.5f, 476.25f);
	WidgetOffset = FVector2D(960.0f, 545.5f);
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

void UTabMinimapWidget::UpdatePlayerPosition(const ETeam& NewTeam,
	const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState)
{
	Super::UpdatePlayerPosition(NewTeam, NewPlayerState);
}

// void UTabMinimapWidget::HidePlayerPosition(const ETeam& NewTeam,
// 	const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState)
// {
// 	Super::HidePlayerPosition(NewTeam, NewPlayerState);
// }
