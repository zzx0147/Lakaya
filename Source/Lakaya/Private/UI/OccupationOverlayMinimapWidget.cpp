// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OccupationOverlayMinimapWidget.h"

#include "Camera/CameraComponent.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/CanvasPanelSlot.h"
#include "GameMode/OccupationGameState.h"

void UOccupationOverlayMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TeamIcons.Emplace(ETeam::Anti, AntiIcon);
	TeamIcons.Emplace(ETeam::Pro, ProIcon);
}

void UOccupationOverlayMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 게임 중이 아닐 때에는 미니맵을 업데이트 해주지 않습니다.
	if (!UpdateMinimap) return;

	// 자신의 팀(자기 자신 포함)위치를 업데이트 해줍니다.
	UpdatePlayerPosition(CurrentTeam);

	// 미니맵 상에 표시되는 적들의 아이콘 회전 값을 업데이트 해줍니다.
	for (const auto& Enemy : OccupationPlayersByMinimap[CurrentEnemyTeam])
	{
		UpdateEnemyImageRotation(Enemy.Value);
	}

	// 미니맵 상에 표시되는 점령 구역 아이콘의 회전 값을 업데이트 해줍니다.
	UpdateAreaImageRotation();

	// 같은 와지가 궁극기를 사용중이라면, 적들의 위치를 업데이트 해줍니다.
	if (const auto& GameState = Cast<AOccupationGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->GetbIsClairvoyanceActivated())
		{
			UpdatePlayerPosition(CurrentEnemyTeam);
			return;
		}
	}

	// 적들의 정보를 가져와서, 팀원(자기 자신 포함)들의 시야에서 적이 보인다면, 미니맵에 표시해줍니다.
	for (const auto& Enemy : OccupationPlayersByMinimap[CurrentEnemyTeam])
	{
		const TWeakObjectPtr<ALakayaBasePlayerState> EnemyState = Enemy.Key;
		const TWeakObjectPtr<UImage> EnemyImage = Enemy.Value;

		for (const auto& Ally : OccupationPlayersByMinimap[CurrentTeam])
		{
			const auto& AllyState = Ally.Key;
			
			ALakayaBaseCharacter* AllyCharacter = Cast<ALakayaBaseCharacter>(AllyState->GetPawn());
			
			if (AllyCharacter->IsEnemyVisibleInCamera(CurrentEnemyTeam, EnemyState, EnemyImage))
			{
				AllyCharacter->Server_OnEnemySpotted(CurrentEnemyTeam, EnemyState.Get());
			}
		}
	}
}

UImage* UOccupationOverlayMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
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
	
	// 나 자신이라면 자신의 팀에 따른 자신만의 아이콘으로 설정해줍니다.
	if (bMyPlayer)
	{
		if (NewTeam == ETeam::Anti)
		{
			PlayerImage->SetBrushFromTexture(AntiOwnIcon);
			return PlayerImage;
		}
		else if (NewTeam == ETeam::Pro)
		{
			PlayerImage->SetBrushFromTexture(ProOwnIcon);
			return PlayerImage;
		}
	}

	// 나 자신이 아니라면 팀에 따라서 아이콘을 설정해줍니다.
	if (TeamIcons.Contains(NewTeam))
	{
		PlayerImage->SetBrushFromTexture(TeamIcons[NewTeam]);
		return PlayerImage;
	}

	return nullptr;
}

void UOccupationOverlayMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	// 자기 자신을 검사해서, 자신의 위치를 토대로 미니맵을 회전과 위치를 업데이트시켜줍니다.
	for (auto& Player : OccupationPlayersByMinimap[Team])
	{
		const auto& State = Player.Key;
		const auto& Image = Player.Value;

		FVector2D PlayerPosition(State->GetPawn()->GetActorLocation().X, State->GetPawn()->GetActorLocation().Y);
		const FVector2D NewPlayerPosition = ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);

		if (Image->GetVisibility() == ESlateVisibility::Hidden)
			Image->SetVisibility(ESlateVisibility::Visible);
		
		// 자기 자신을 검사해서, 자신의 위치를 토대로 미니맵의 회전과 위치를 업데이트시켜줍니다.
		if (State == GetOwningPlayerState())
		{
			UpdateMinimapImagePositionAndRotation(*State, NewPlayerPosition);

			const auto PlayerCharacter = State->GetPlayerController()->GetCharacter();
			const auto LakayaCharacter = Cast<ALakayaBaseCharacter>(PlayerCharacter);
			const FRotator PlayerRotation = LakayaCharacter->GetCamera()->GetComponentRotation();
			
			Image->SetRenderTransformAngle(PlayerRotation.Yaw + 90.0f);
		}

		Image->SetRenderTranslation(NewPlayerPosition + WidgetOffset);
		
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
	for (auto& Player : OccupationPlayersByMinimap[CurrentEnemyTeam])
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
					Image->SetBrushFromTexture(DeathIcon);
			}
		}
	}
}

void UOccupationOverlayMinimapWidget::SetEnemyImage() const
{
	for (const auto& Enemy : OccupationPlayersByMinimap[CurrentTeam == ETeam::Anti ? ETeam::Pro : ETeam::Anti])
	{
		const auto& EnemyImage = Enemy.Value;
		EnemyImage->SetBrushFromTexture(QuestionMarkIcon);
	}
}

void UOccupationOverlayMinimapWidget::UpdatePlayerPosition(const ETeam& NewTeam,
                                                       const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState)
{
#pragma region NullCheck
	if (const TWeakObjectPtr<ALakayaBasePlayerState> WeakNewPlayerState = NewPlayerState; !OccupationPlayersByMinimap[NewTeam].Contains(WeakNewPlayerState))
	{
		UE_LOG(LogTemp, Warning, TEXT("NewPlayerState is not in PlayersByMinimap."));
		return;
	}
	
	const auto& EnemyImage = OccupationPlayersByMinimap[NewTeam][NewPlayerState].Get();
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

	FTimerHandle NewTimerHandle;
	if (PlayerTimers.Contains(NewPlayerState))
		GetWorld()->GetTimerManager().ClearTimer(PlayerTimers[NewPlayerState]);
	
	GetWorld()->GetTimerManager().SetTimer(NewTimerHandle, [this, EnemyImage, NewTeam]()
	{
		for (const auto& Enemy : OccupationPlayersByMinimap[NewTeam])
		{
			SetEnemyImage();
		}
	}, 0.1f, false);
	SetPlayerTimers(NewPlayerState, NewTimerHandle);
}

void UOccupationOverlayMinimapWidget::UpdateAreaImageRotation()
{
	const auto PlayerCharacter = Cast<ALakayaBaseCharacter>(GetOwningPlayerPawn());
	const FRotator PlayerRotation = PlayerCharacter->GetCamera()->GetComponentRotation();

	AntiAreaImage->SetRenderTransformAngle(PlayerRotation.Yaw + 90.0f);
	CenterAreaImage->SetRenderTransformAngle(PlayerRotation.Yaw + 90.0f);
	ProAreaImage->SetRenderTransformAngle(PlayerRotation.Yaw + 90.0f);
}
