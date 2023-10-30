// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IndividualOverlayMinimapWidget.h"

#include "Character/LakayaBaseCharacter.h"
#include "Components/CanvasPanelSlot.h"

void UIndividualOverlayMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ParentPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("RetainerCanvasPanel")));
	// RetainerBox = Cast<URetainerBox>(GetWidgetFromName(TEXT("RetainerBox_74")));
}

void UIndividualOverlayMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const TWeakObjectPtr<ALakayaBasePlayerState> MyPlayerState = Cast<ALakayaBasePlayerState>(GetOwningPlayerState());

	// 자기 자신의 위치를 업데이트 해줍니다.
	UpdatePlayerPosition(MyPlayerState);
}

UImage* UIndividualOverlayMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
{
	UImage* PlayerImage = NewObject<UImage>(this);
	UCanvasPanelSlot* PanelSlot = ParentPanel->AddChildToCanvas(PlayerImage);

	if (PanelSlot == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("PanelSlot is null."));
		return nullptr;
	}

	PanelSlot->SetAlignment(IconAlignment);
	PanelSlot->SetSize(IconSize);
	PanelSlot->SetZOrder(1);

	PlayerImage->SetVisibility(ESlateVisibility::Hidden);

	if (bMyPlayer)
		PlayerImage->SetBrushFromTexture(IndividualOwnIcon);
	else
		PlayerImage->SetBrushFromTexture(IndividualEnemyIcon);
	
	return nullptr;
}

void UIndividualOverlayMinimapWidget::UpdatePlayerPosition(const ETeam& NewTeam,
	const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState)
{
	// Super::UpdatePlayerPosition(NewTeam, NewPlayerState);

	// const auto& MyImage = EnemiesByMinimap[NewTeam][NewPlayerState];
	
	// 나 자신의 위치를 업데이트 해주면서 미니맵 위치와 회전을 조절합니다.
	if (NewPlayerState == GetOwningPlayerState())
	{
		const FVector2D PlayerPosition(NewPlayerState->GetPawn()->GetActorLocation().X, NewPlayerState->GetPawn()->GetActorLocation().Y);
		const FVector2D NewPlayerPosition = ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);

		UpdateMinimapImagePositionAndRotation(*NewPlayerState, NewPlayerPosition);
	}

	// TODO : 자기 자신의 위치를 미님맵상에 표시해줘야 합니다.
	
	// 나 자신을 검사해서 죽어있다면, 죽음 아이콘으로 변경해줍니다.
	// if (const auto LakayaCharacter = Cast<ALakayaBaseCharacter>(NewPlayerState->GetPawn()))
	// {
	// 	if (!LakayaCharacter->GetAliveState())
	// 	{
	// 		
	// 	}
	// }
}

FVector2d UIndividualOverlayMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
	const FVector2D& MiniMapSize)
{
	return Super::ConvertWorldToMiniMapCoordinates(PlayerLocation, MiniMapSize);
}

void UIndividualOverlayMinimapWidget::UpdateMinimapImagePositionAndRotation(
	const ALakayaBasePlayerState& NewPlayerState, const FVector2D NewPosition) const
{
	Super::UpdateMinimapImagePositionAndRotation(NewPlayerState, NewPosition);
}

void UIndividualOverlayMinimapWidget::UpdatePlayerPosition(const TWeakObjectPtr<ALakayaBasePlayerState>& NewPlayerState)
{
	const FVector2D WorldPlayerPosition(NewPlayerState->GetPawn()->GetActorLocation().X, NewPlayerState->GetPawn()->GetActorLocation().Y);
	const FVector2D ConvertByMinimapPlayerPosition = ConvertWorldToMiniMapCoordinates(WorldPlayerPosition, MinimapSize);

	// TODO : IndividualPlayerByMinimap을 업데이트 해서 자기 자신의 이미지를 가져와야 합니다.
}