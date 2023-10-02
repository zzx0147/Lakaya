// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OccupationMinimapWidget.h"

#include "EngineUtils.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void UOccupationMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MinimapImage = Cast<UImage>(GetWidgetFromName(TEXT("MinimapImage")));
	CanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("CanvasPanel")));
	
	UpdateMinimap = false;
}

void UOccupationMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 게임 중에는 미니맵을 실시간으로 업데이트해줍니다.
	if (UpdateMinimap)
	{
		// TODO : 자기 자신의 팀만을 업데이트 해줘야합니다. (예외로 와지 궁을 사용했을 시에는 모두의 팀을 업데이트 해줘야합니다.)
		if (CurrentTeam == ETeam::Anti)
		{
			UpdatePlayerPosition(ETeam::Anti);
		}
		else if (CurrentTeam == ETeam::Pro)
		{
			UpdatePlayerPosition(ETeam::Pro);
		}
	}
}

void UOccupationMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	if (!MinimapImage.IsValid()) return;
	
	// MinimapResource Size
	const FVector2D MinimapSize(250.0f, 381.0f);
	
	for (auto& Player : PlayersByMinimap[Team])
	{
		const auto& State = Player.Key;
		const auto& Image = Player.Value;

		FVector2D PlayerPosition(State->GetPawn()->GetActorLocation().X, State->GetPawn()->GetActorLocation().Y);

		const FVector2D NewPlayerPosition = ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);

		if (Image->GetVisibility() == ESlateVisibility::Hidden)
			Image->SetVisibility(ESlateVisibility::Visible);
		
		Image->SetRenderTranslation(NewPlayerPosition + FVector2D(225.0f, 250.5f));
	}
}

UImage* UOccupationMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, bool bMyPlayer)
{
	UImage* NewImage = NewObject<UImage>(this);
	const auto Team = NewTeam == ETeam::Anti ? ETeam::Anti : ETeam::Pro;

	UCanvasPanelSlot* PanelSlot = CanvasPanel->AddChildToCanvas(NewImage);

	PanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	PanelSlot->SetSize(FVector2D(12.0f, 12.0f));
	
	// NewImage->SetVisibility(ESlateVisibility::Visible);
	NewImage->SetVisibility(ESlateVisibility::Hidden);

	// 나 자신이라면 자신만의 아이콘으로 설정해줍니다.
	if (bMyPlayer)
	{
		NewImage->SetBrushFromTexture(MinimapOwnIcon);
		return NewImage;
	}

	// 이미지를 생성한 플레이어의 팀에 따라 아이콘을 바꿔줍니다.
	if (Team == ETeam::Anti)
	{
		NewImage->SetBrushFromTexture(MinimapAntiIcon);
	}
	else if (Team == ETeam::Pro)
	{
		NewImage->SetBrushFromTexture(MinimapProIcon);	
	}

	return NewImage;
}

FVector2D UOccupationMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
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