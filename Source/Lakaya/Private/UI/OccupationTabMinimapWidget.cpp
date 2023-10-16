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
}

void UOccupationTabMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	Super::UpdatePlayerPosition(Team);

	// 자기 자신을 검사해서, 자신의 위치를 토대로 미니맵을 회전과 위치를 업데이트시켜줍니다.
	for (auto& Player : PlayersByMinimap[Team])
	{
		const auto& State = Player.Key;
		const auto& Image = Player.Value;

		// 자기 자신을 검사해서, 자신의 위치를 토대로 미니맵의 회전과 위치를 업데이트시켜줍니다.
		if (State == GetOwningPlayerState())
		{
			FVector2D PlayerPosition(State->GetPawn()->GetActorLocation().X, State->GetPawn()->GetActorLocation().Y);
			const FVector2D NewPlayerPosition = ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);

			// UpdateMinimapImagePositionAndRotation(*State, NewPlayerPosition);
		}

		// 아군들을 검사해서 아군(나 자신 포함)이 죽어있다면, 죽음 아이콘으로 변경해줍니다.
		if (const auto PlayerCharacter = State->GetPawn())
		{
			if (const ALakayaBaseCharacter* LakayaCharacter = Cast<ALakayaBaseCharacter>(PlayerCharacter))
			{
				// 검사한 아군(나 자신 포함)이 죽었다면 텍스처를 DeathIcon으로 변경해줍니다.
				if (!LakayaCharacter->GetAliveState())
				{
					Image->SetBrushFromTexture(DeathIcon);
				}
				else 
				{
					switch (Team)
					{
					case ETeam::Anti:
						if (State == GetOwningPlayerState())
						{
							Image->SetBrushFromTexture(AntiOwnIcon);
							break;
						}
						Image->SetBrushFromTexture(AntiIcon);
						break;
					case ETeam::Pro:
						if (State == GetOwningPlayerState())
						{
							Image->SetBrushFromTexture(ProOwnIcon);
							break;
						}
						Image->SetBrushFromTexture(ProIcon);
						break;
					default:
						break;
					}
				}
			}
		}
	}
}

void UOccupationTabMinimapWidget::UpdatePlayerPosition(const ETeam& NewTeam,
	const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState)
{
#pragma region NullCheck
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
#pragma endregion
	const FVector2D PlayerPosition(NewPlayerState->GetPawn()->GetActorLocation().X, NewPlayerState->GetPawn()->GetActorLocation().Y);
	const FVector2D NewPlayerPosition = const_cast<UOccupationTabMinimapWidget*>(this)->ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);

	if (EnemyImage->GetVisibility() == ESlateVisibility::Visible)
	{
		if (NewTeam == ETeam::Anti)
			EnemyImage->SetBrushFromTexture(AntiIcon);
		else if (NewTeam == ETeam::Pro)
			EnemyImage->SetBrushFromTexture(ProIcon);
	}
	
	if (EnemyImage->GetVisibility() == ESlateVisibility::Hidden)
		EnemyImage->SetVisibility(ESlateVisibility::Visible);

	EnemyImage->SetRenderTranslation(NewPlayerPosition + WidgetOffset);
}

void UOccupationTabMinimapWidget::HidePlayerPosition(const ETeam& NewTeam,
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

	if (EnemyImage->GetVisibility() == ESlateVisibility::Visible)
	EnemyImage->SetVisibility(ESlateVisibility::Hidden);

}

UImage* UOccupationTabMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
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

	// 나 자신이라면 자신의 팀에 따른 자신만의 아이콘으로 설정해줍니다.
	if (bMyPlayer)
	{
		if (Team == ETeam::Anti)
		{
			PlayerImage->SetBrushFromTexture(AntiOwnIcon);
			return PlayerImage;
		}
		else if (Team == ETeam::Pro)
		{
			PlayerImage->SetBrushFromTexture(ProOwnIcon);
			return PlayerImage;
		}
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
