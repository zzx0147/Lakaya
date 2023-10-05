// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/OccupationTabMinimapWidget.h"

#include "Components/CanvasPanelSlot.h"

void UOccupationTabMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MinimapImage == nullptr) UE_LOG(LogTemp, Warning, TEXT("OccupationTabMinimapWidget_MinimapImage is null."));	

	ParentPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("PlayerImagePanel")));
	
	TeamIcons.Emplace(ETeam::Anti, AntiIcon);
	TeamIcons.Emplace(ETeam::Pro, ProIcon);

	// TODO : 하드코딩이 아닌 GetDesiredSize() 함수를 이용해서 가져오도록 해야합니다.
	MinimapSize = FVector2D(312.5f, 476.25f);
}

void UOccupationTabMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 게임 중에는 미니맵을 프레임마다업데이트 해줍니다.
	if (UpdateMinimap)
	{
		UpdatePlayerPosition(CurrentTeam);
	}
}

void UOccupationTabMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	Super::UpdatePlayerPosition(Team);

	if (!MinimapImage) return;

	for (auto& Player : PlayersByMinimap[Team])
	{
		const auto& State = Player.Key;
		const auto& Image = Player.Value;

		FVector2D PlayerPosition(State->GetPawn()->GetActorLocation().X, State->GetPawn()->GetActorLocation().Y);

		const FVector2D NewPlayerPosition = ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);

		if (Image->GetVisibility() == ESlateVisibility::Hidden)
			Image->SetVisibility(ESlateVisibility::Visible);

		// TODO : 맵과 이미지 사이즈가 확정이 되면 수정해야 합니다.
		// NewPlayerPosition + Widget Position = Player Position
		Image->SetRenderTranslation(NewPlayerPosition + FVector2D(1520.0f, 540.5f));
	}
}

UImage* UOccupationTabMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
{
	UImage* PlayerImage = NewObject<UImage>(this);
	const auto Team = NewTeam == ETeam::Anti ? ETeam::Anti : ETeam::Pro;

	UCanvasPanelSlot* PanelSlot = ParentPanel->AddChildToCanvas(PlayerImage);

	PanelSlot->SetAlignment(IconAlignment);
	PanelSlot->SetSize(IconSize);

	PlayerImage->SetVisibility(ESlateVisibility::Hidden);

	// 나 자신이라면 자신만의 아이콘으로 설정해줍니다.
	if (bMyPlayer)
	{
		PlayerImage->SetBrushFromTexture(OwnIcon);
		return PlayerImage;
	}

	// 생성된 이미지는 플레이어의 팀에 따라 아이콘이 정해집니다.
	if (TeamIcons.Contains(Team))
	{
		PlayerImage->SetBrushFromTexture(TeamIcons[Team]);
		return PlayerImage;
	}

	UE_LOG(LogTemp, Warning, TEXT("OccupationTabMinimapWidget_PlayerImage is null."));
	return nullptr;
}

FVector2D UOccupationTabMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
                                                                        const FVector2D& MiniMapSize)
{
	return Super::ConvertWorldToMiniMapCoordinates(PlayerLocation, MiniMapSize);
}