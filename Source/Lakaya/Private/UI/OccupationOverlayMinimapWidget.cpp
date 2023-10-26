// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OccupationOverlayMinimapWidget.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/CanvasPanelSlot.h"
#include "GameMode/OccupationGameState.h"

void UOccupationOverlayMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ParentPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("RetainerCanvasPanel")));

	TeamIcons.Emplace(ETeam::Anti, AntiIcon);
	TeamIcons.Emplace(ETeam::Pro, ProIcon);

	OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
}

void UOccupationOverlayMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
#pragma region 
	if (!OccupationGameState /* || !AllyUpdateMinimap || !OwnerCharacter */)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameState is null."));
		return;
	}

	if (!AllyUpdateMinimap)
	{
		UE_LOG(LogTemp, Warning, TEXT("AllyUpdateMinimap is false."));
		return;
	}
#pragma endregion
	
	// 자신의 팀(자기 자신 포함)위치를 업데이트 해줍니다.
	UpdatePlayerPosition(CurrentTeam);

	// 게임이 본격적으로 게임되지 않은 상태라면, 적의 위치를 업데이트 해주지 않습니다.
	if (!EnemyUpdateMinimap) return;
	
	// 와지가 궁극기 스킬을 사용 중이라면, 적들의 정보 순회를 멈추고 모든 적의 위치를 업데이트 해줍니다.
	if (OccupationGameState->GetbIsClairvoyanceActivated())
	{
		UpdatePlayerPosition(EnemyTeam);
		return;
	}
	
	// 적들을 순회해서, 시야에 들어왔는 지 검사를 합니다.
	for (const auto& Enemy : PlayersByMinimap[EnemyTeam])
	{
		const TWeakObjectPtr<ALakayaBasePlayerState> EnemyState = Enemy.Key;
		const TWeakObjectPtr<UImage> EnemyMinimapImage = Enemy.Value;

		if (EnemyState->GetSpotted())
		{
			UpdatePlayerPosition(EnemyTeam, EnemyState);
			return;
		}
		
		if (const ALakayaBaseCharacter* EnemyCharacter = EnemyState->GetPawn<ALakayaBaseCharacter>())
		{
			const bool IsRendered = EnemyCharacter->WasRecentlyRendered(0.1f);
			if (IsRendered && !EnemyState->GetSpotted())
			{
				EnemyState->SetSpotted(true);
				UE_LOG(LogTemp, Warning, TEXT("Enemy Spotted."));
			}
		}
		
		// 해당 적을 아군이 렌더링 중이라면 리스트에 추가해줍니다.
		// for (const auto& Ally : PlayersByMinimap[CurrentTeam])
		// {
		// 	const auto& AllyState = Ally.Key;
		// 	const auto& AllyCharacter = Cast<ALakayaBaseCharacter>(AllyState->GetPawn());
		// 	if (AllyCharacter->IsEnemySpotted(EnemyState))
		// 	{
		// 		
		// 		
		// 		if (!OccupationGameState->SpottedPlayers.Contains(EnemyState))
		// 		{
		// 			// OccupationGameState->SpottedPl33333333 3ayers.Emplace(EnemyState);
		// 			UE_LOG(LogTemp, Warning, TEXT("Emplace"));
		// 		}
		// 	}
		// }

		// if (!bIsEnemySpotted && OccupationGameState->SpottedPlayers.Contains(EnemyState))
		// {
		// 	// OccupationGameState->SpottedPlayers.Remove(EnemyState);
		// 	UE_LOG(LogTemp, Warning, TEXT("Remove"));
		// }
	}

	// SpottedPlayers를 검사해서 적이 리스트에 있다면 적의 위치를 업데이트 해줍니다.
	// for (const auto& Player : OccupationGameState->SpottedPlayers)
	// {
	// 	if (Player->GetTeam() == CurrentTeam) return;
	//
	// 	UpdatePlayerPosition(EnemyTeam, Player);
	// }

	// UE_LOG(LogTemp, Warning, TEXT("%d"), OccupationGameState->SpottedPlayers.Num());
}

FVector2d UOccupationOverlayMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
	const FVector2D& MiniMapSize)
{
	return Super::ConvertWorldToMiniMapCoordinates(PlayerLocation, MiniMapSize);
}

UImage* UOccupationOverlayMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
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

	// 나 자신이 아니라면 팀에 따라서 아이콘을 설정해줍니다.
	if (TeamIcons.Contains(Team))
	{
		PlayerImage->SetBrushFromTexture(TeamIcons[Team]);
		return PlayerImage;
	}

	return nullptr;
}

void UOccupationOverlayMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
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

			UpdateMinimapImagePositionAndRotation(*State, NewPlayerPosition);
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

	// 적 사망 검사
	for (auto& Player : PlayersByMinimap[Team == ETeam::Anti ? ETeam::Pro : ETeam::Anti])
	{
		const auto& State = Player.Key;
		const auto& Image = Player.Value;

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
					switch (Team == ETeam::Anti ? ETeam::Pro : ETeam::Anti)
					{
					case ETeam::Anti:
						// Image->SetBrushFromTexture(AntiIcon);
						break;
					case ETeam::Pro:
						// Image->SetBrushFromTexture(ProIcon);
						break;
					default:
						break;
					}
				}
			}
		}
	}
}

void UOccupationOverlayMinimapWidget::SetEnemyImage() const
{
	for (const auto& Enemy : PlayersByMinimap[CurrentTeam == ETeam::Anti ? ETeam::Pro : ETeam::Anti])
	{
		const auto& EnemyImage = Enemy.Value;
		EnemyImage->SetBrushFromTexture(QuestionMarkIcon);
	}
	;
}

void UOccupationOverlayMinimapWidget::UpdatePlayerPosition(const ETeam& NewTeam,
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
	const FVector2D NewPlayerPosition = const_cast<UOccupationOverlayMinimapWidget*>(this)->ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);

	// if (EnemyImage->GetVisibility() == ESlateVisibility::Visible)
	// {
	// 	if (NewTeam == ETeam::Anti)
	// 		EnemyImage->SetBrushFromTexture(AntiIcon);
	// 	else if (NewTeam == ETeam::Pro)
	// 		EnemyImage->SetBrushFromTexture(ProIcon);
	// }

	if (EnemyImage->GetVisibility() == ESlateVisibility::Hidden)
		EnemyImage->SetVisibility(ESlateVisibility::Visible);

	// if (NewTeam == ETeam::Anti)
	// {
	// 	EnemyImage->SetBrushFromTexture(AntiIcon);
	// }
	// else
	// {
	// 	EnemyImage->SetBrushFromTexture(ProIcon);
	// }
	
	EnemyImage->SetRenderTranslation(NewPlayerPosition + WidgetOffset);

	// GetWorld()->GetTimerManager().ClearTimer(QuestionIconTimerHandle);
	
	// GetWorld()->GetTimerManager().SetTimer(QuestionIconTimerHandle, [this, EnemyImage, NewTeam]()
	// {
	// 	for (const auto& Enemy : PlayersByMinimap[NewTeam])
	// 	{
	// 		SetEnemyImage();
	// 		UE_LOG(LogTemp, Warning, TEXT("타이머 호출"));
	// 	}
	// }, 0.1f, false);
}

void UOccupationOverlayMinimapWidget::HidePlayerPosition(const ETeam& NewTeam,
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

void UOccupationOverlayMinimapWidget::UpdateMinimapImagePositionAndRotation(const ALakayaBasePlayerState& NewPlayerState,
                                                                        const FVector2D NewPosition) const
{
	Super::UpdateMinimapImagePositionAndRotation(NewPlayerState, NewPosition);
}
