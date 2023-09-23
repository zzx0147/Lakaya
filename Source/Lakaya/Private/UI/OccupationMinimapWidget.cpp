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
	// PlayerImage = Cast<UImage>(GetWidgetFromName(TEXT("PlayerImage")));
	CanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("CanvasPanel")));
	
	UpdateMinimap = false;
}

void UOccupationMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// TODO : 자신의 위치를 넘기는 것이 아닌, 모든 플레이어의 위치를 넘겨줘야 합니다.
	// 게임 중에는 미니맵을 실시간으로 업데이트해줍니다.
	if (UpdateMinimap)
	{
		// const auto PlayerActor = GetOwningPlayer()->GetPawn();
		// if (PlayerActor)
		// {
		// 	const FVector2D PlayerLocation(PlayerActor->GetActorLocation().X, PlayerActor->GetActorLocation().Y);
		// 	UpdatePlayerPosition(PlayerLocation);
		// }
		UpdatePlayerPosition();
	}
}

void UOccupationMinimapWidget::UpdatePlayerPosition()
{
	// MinimapResource Size
	const FVector2D MiniMapSize(250.0f, 381.0f);

	for (auto& Player : PlayersByMinimap[ETeam::Anti])
	{
		auto& State = Player.Key;
		auto& Image = Player.Value;

		FVector2D PlayerPosition(State->GetPawn()->GetActorLocation().X, State->GetPawn()->GetActorLocation().Y);
		
		// 월드 상에 존재하는 본인의 위치를 미니맵 좌표로 전환합니다.
		const FVector2D NewPlayerPosition = ConvertWorldToMiniMapCoordinates(PlayerPosition, MiniMapSize);

		Image->SetRenderTranslation(NewPlayerPosition + FVector2D(225.0f, 250.5f));
	}

	for (auto& Player : PlayersByMinimap[ETeam::Pro])
	{
		auto& State = Player.Key;
		auto& Image = Player.Value;

		FVector2D PlayerPosition(State->GetPawn()->GetActorLocation().X, State->GetPawn()->GetActorLocation().Y);
		
		// 월드 상에 존재하는 본인의 위치를 미니맵 좌표로 전환합니다.
		const FVector2D NewPlayerPosition = ConvertWorldToMiniMapCoordinates(PlayerPosition, MiniMapSize);

		Image->SetRenderTranslation(NewPlayerPosition + FVector2D(225.0f, 250.5f));
	}

	// TODO : PlayersByMinimap에서 자신의 팀을 전부 순회하면서 각각의 플레이어의 아이콘의 위치를 업데이트 시켜줘야 합니다.
	// 전환된 좌표로 플레이어의 위치를 옮겨줍니다.
	// FVector2D = ((MinimapPosition.x + MinimapSize / 2), (MinimapPosition.y + MinimapSize / 2)
	// PlayerImage->SetRenderTranslation(NewPlayerPosition + FVector2D(225.0f, 250.5f));
}

UImage* UOccupationMinimapWidget::CreatePlayerImage(const ETeam& NewTeam)
{
	UImage* NewImage = NewObject<UImage>(this);
	const auto Team = NewTeam == ETeam::Anti ? ETeam::Anti : ETeam::Pro;

	UCanvasPanelSlot* PanelSlot = CanvasPanel->AddChildToCanvas(NewImage);
	if (PanelSlot == nullptr) UE_LOG(LogTemp, Warning, TEXT("PanelSlot is nullptr."));

	PanelSlot->SetAlignment(FVector2D(0.5f, 0.5f));
	PanelSlot->SetSize(FVector2D(12.0f, 12.0f));
	
	NewImage->SetVisibility(ESlateVisibility::Visible);
	
	// 미니맵의 가운데 좌표 : FVector(225,0f, 250.0f)
	NewImage->SetRenderTranslation(FVector2D(225.0f, 250.0f));

	if (Team == ETeam::Anti)
	{
		NewImage->SetBrushFromTexture(MinimapAntiIcon);
		UE_LOG(LogTemp, Warning, TEXT("Anti Icon"));
	}
	else if (Team == ETeam::Pro)
	{
		NewImage->SetBrushFromTexture(MinimapProIcon);	
		UE_LOG(LogTemp, Warning, TEXT("Pro Icon"));
	}
	
	return NewImage;
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
