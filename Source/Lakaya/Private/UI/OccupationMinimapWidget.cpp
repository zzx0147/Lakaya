// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OccupationMinimapWidget.h"

#include "EngineUtils.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"

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

	// 게임 중에는 미니맵을 실시간으로 업데이트해줍니다.
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

	// 월드 상에 존재하는 본인의 위치를 미니맵 좌표로 전환합니다.
	const FVector2D NewPlayerPosition = ConvertWorldToMiniMapCoordinates(PlayerPosition, MiniMapSize);

	// TODO : 테스트를 위해 주석처리 했습니다.
	// 전환된 좌표로 플레이어의 위치를 옮겨줍니다.
	// FVector2D = ((MinimapPosition.x + MinimapSize / 2), (MinimapPosition.y + MinimapSize / 2)
	PlayerImage->SetRenderTranslation(NewPlayerPosition + FVector2D(225.0f, 250.5f));
}

// UImage* UOccupationMinimapWidget::CreatePlayerImage(const ETeam& NewTeam)
// {
// 	const auto NewImage = NewObject<UImage>(this);
// 	const auto Team = NewTeam == ETeam::Anti ? ETeam::Anti : ETeam::Anti;
//
// 	if (Team == ETeam::Anti)
// 	{
// 		NewImage->SetBrushFromTexture(MinimapAntiIcon);
// 	}
// 	else if (Team == ETeam::Pro)
// 	{
// 		NewImage->SetBrushFromTexture(MinimapProIcon);	
// 	}
//
// 	// 미니맵의 가운데 좌표 : FVector(225,0f, 250.0f)
// 	NewImage->SetRenderTranslation(FVector2D(225.0f, 250.0f));
//
// 	MinimapByPlayers.Emplace(Team, NewImage);
// 	
// 	NewImage->SetVisibility(ESlateVisibility::Visible);
// 	
// 	return NewImage;
// }

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
