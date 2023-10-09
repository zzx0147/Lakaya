// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/OccupationTabMinimapWidget.h"

#include "Character/LakayaBaseCharacter.h"
#include "Components/CanvasPanelSlot.h"

void UOccupationTabMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ParentPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("PlayerImagePanel")));
	
	TeamIcons.Emplace(ETeam::Anti, AntiIcon);
	TeamIcons.Emplace(ETeam::Pro, ProIcon);

	// TODO : 하드코딩이 아닌 GetDesiredSize() 함수를 이용해서 가져오도록 해야합니다.
	MinimapSize = FVector2D(312.5f, 476.25f);

	WidgetOffset = FVector2D(1515.0f, 545.5f);
}

void UOccupationTabMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
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

void UOccupationTabMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	Super::UpdatePlayerPosition(Team);

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
		Image->SetRenderTranslation(NewPlayerPosition + WidgetOffset);
	}
}

void UOccupationTabMinimapWidget::UpdatePlayerPosition(const ETeam& NewTeam,
	const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState)
{
	if (const TWeakObjectPtr<ALakayaBasePlayerState> WeakNewPlayerState = NewPlayerState; !PlayersByMinimap[NewTeam].Contains(WeakNewPlayerState))
	{
		UE_LOG(LogTemp, Warning, TEXT("NewPlayerState is not in PlayersByMinimap."));
		return;
	}

	const auto& EnemyImage = PlayersByMinimap[NewTeam][NewPlayerState].Get();
	if (EnemyImage == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyImage is null."));
		return;
	}
	
	const FVector2D PlayerPosition(NewPlayerState->GetPawn()->GetActorLocation().X, NewPlayerState->GetPawn()->GetActorLocation().Y);
	const FVector2D NewPlayerPosition = const_cast<UOccupationTabMinimapWidget*>(this)->ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);

	if (EnemyImage->GetVisibility() == ESlateVisibility::Hidden)
		EnemyImage->SetVisibility(ESlateVisibility::Visible);

	EnemyImage->SetRenderTranslation(NewPlayerPosition + WidgetOffset);
}

UImage* UOccupationTabMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
{
	UImage* PlayerImage = NewObject<UImage>(this);
	const auto Team = NewTeam == ETeam::Anti ? ETeam::Anti : ETeam::Pro;

	UCanvasPanelSlot* PanelSlot = ParentPanel->AddChildToCanvas(PlayerImage);

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

FVector2D UOccupationTabMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
                                                                        const FVector2D& MiniMapSize)
{
	return Super::ConvertWorldToMiniMapCoordinates(PlayerLocation, MiniMapSize);
}