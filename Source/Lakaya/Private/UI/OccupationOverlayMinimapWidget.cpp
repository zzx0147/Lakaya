// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OccupationOverlayMinimapWidget.h"

#include "Character/LakayaBaseCharacter.h"
#include "Components/CanvasPanelSlot.h"

void UOccupationOverlayMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ParentPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("RetainerCanvasPanel")));
	RetainerBox = Cast<URetainerBox>(GetWidgetFromName(TEXT("RetainerBox_74")));

	TeamIcons.Emplace(ETeam::Anti, AntiIcon);
	TeamIcons.Emplace(ETeam::Pro, ProIcon);
	
	MinimapSize = FVector2D(250.0f, 381.0f);
	WidgetOffset = FVector2D(125.0f, 127.5f);
}

void UOccupationOverlayMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
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

	if (EnemyImage->GetVisibility() == ESlateVisibility::Visible)
	{
		if (NewTeam == ETeam::Anti)
			EnemyImage->SetBrushFromTexture(AntiIcon);
		else if (NewTeam == ETeam::Pro)
			EnemyImage->SetBrushFromTexture(ProIcon);
	}

	if (EnemyImage->GetVisibility() == ESlateVisibility::Hidden)
		EnemyImage->SetVisibility(ESlateVisibility::Visible);

	if (NewTeam == ETeam::Anti)
	{
		EnemyImage->SetBrushFromTexture(AntiIcon);
	}
	else
	{
		EnemyImage->SetBrushFromTexture(ProIcon);
	}
	
	EnemyImage->SetRenderTranslation(NewPlayerPosition + WidgetOffset);

	// GetWorld()->GetTimerManager().ClearTimer(QuestionIconTimerHandle);

	FTimerHandle NewTimerHandle;
	
	if (PlayerTimers.Contains(NewPlayerState))
	{
		GetWorld()->GetTimerManager().ClearTimer(PlayerTimers[NewPlayerState]);
	}
	
	GetWorld()->GetTimerManager().SetTimer(NewTimerHandle, [this, EnemyImage, NewTeam]()
	{
		for (const auto& Enemy : PlayersByMinimap[NewTeam])
		{
			SetEnemyImage();
			UE_LOG(LogTemp, Warning, TEXT("타이머 호출"));
		}
	}, 0.1f, false);

	SetPlayerTimers(NewPlayerState, NewTimerHandle);
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
