// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MiniMapWidget.h"

#include "Character/LakayaBaseCharacter.h"
#include "Components/CanvasPanelSlot.h"

void UMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TeamIcons.Emplace(ETeam::Anti, AntiIcon);
	TeamIcons.Emplace(ETeam::Pro, ProIcon);
	
	UpdateMinimap = false;
	
	IconAlignment = FVector2D(0.5f, 0.5f);
	IconSize = FVector2D(12.0f, 12.0f);

	PlayersByMinimap.Emplace(ETeam::Anti);
	PlayersByMinimap.Emplace(ETeam::Pro);
}

void UMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 게임 중이 아닐때에는 미니맵을 업데이트 해주지 않습니다.
	if (!UpdateMinimap) return;

	// 자기 자신의 팀의 위치를 업데이트 해줍니다.
	UpdatePlayerPosition(CurrentTeam);

	const ETeam EnemyTeam = CurrentTeam == ETeam::Anti ? ETeam::Pro : ETeam::Anti;

	// TODO : 와지 투시 스킬을 사용했을 때도 추가해줘야 합니다.
	// 적들의 위치정보를 가져와서, 내 시야에 들어와있다면, 미니맵에 표시해줍니다.
	for (const auto& Enemy : PlayersByMinimap[EnemyTeam])
	{
		const auto& State = Enemy.Key;
		
		if (const ALakayaBaseCharacter* LakayaCharacter = Cast<ALakayaBaseCharacter>(State->GetPawn()))
		{
			bool bIsEnemyVisibleInCamera = LakayaCharacter->IsEnemyVisibleInCamera(State);
			
			if (bIsEnemyVisibleInCamera)
			{
				UpdatePlayerPosition(EnemyTeam, State);
				continue;
			}
			
			bool bIsCurrentlyVisibleOnMinimap = Enemy.Value->GetVisibility() != ESlateVisibility::Hidden;
			
			if(bIsCurrentlyVisibleOnMinimap)
				Enemy.Value->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

UImage* UMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
{
	UImage* PlayerImage = NewObject<UImage>(this);
	const auto Team = NewTeam == ETeam::Anti ? ETeam::Anti : ETeam::Pro;

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

void UMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	for (auto& Player : PlayersByMinimap[Team])
	{
		const auto& State = Player.Key;
		const auto& Image = Player.Value;

		FVector2D PlayerPosition(State->GetPawn()->GetActorLocation().X, State->GetPawn()->GetActorLocation().Y);

		const FVector2D NewPlayerPosition = ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);

		if (Image->GetVisibility() == ESlateVisibility::Hidden)
			Image->SetVisibility(ESlateVisibility::Visible);

		Image->SetRenderTranslation(NewPlayerPosition + WidgetOffset);
	}
}
