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
	if (UpdateMinimap == false) return;

	// 자신의 팀(자기 자신 포함)위치를 업데이트 해줍니다.
	UpdatePlayerPosition(CurrentTeam);

	// 미니맵 상에 표시되는 적들의 아이콘 회전 값을 업데이트 해줍니다.
	for (const auto Enemy : OccupationPlayersByMinimap[CurrentEnemyTeam])
		UpdateEnemyImageRotation(Enemy.Value);

	// 미니맵 상에 표시되는 점령 구역 아이콘의 회전 값을 업데이트 해줍니다.
	UpdateAreaImageRotation();

	// 같은 와지가 궁극기를 사용중이라면, 적들의 위치를 업데이트 해줍니다.
	if (const auto GameState = Cast<AOccupationGameState>(GetWorld()->GetGameState()))
	{
		if (GameState->GetbIsClairvoyanceActivated())
		{
			UpdatePlayerPosition(CurrentEnemyTeam);
			return;
		}
	}

	// 적들의 정보를 가져와서, 팀원(자기 자신 포함)들의 시야에서 적이 보인다면, 미니맵에 표시해줍니다.
	for (const auto Enemy : OccupationPlayersByMinimap[CurrentEnemyTeam])
	{
		const TWeakObjectPtr<ALakayaBasePlayerState> EnemyState = Enemy.Key;
		const TWeakObjectPtr<UImage> EnemyImage = Enemy.Value;

		if (!EnemyState.IsValid() || !EnemyImage.IsValid()) return;

		for (const auto Ally : OccupationPlayersByMinimap[CurrentTeam])
		{
			auto AllyState = Ally.Key;

			if (!AllyState.IsValid()) return;
			
			ALakayaBaseCharacter* AllyCharacter = Cast<ALakayaBaseCharacter>(AllyState->GetPawn());

			if (!IsValid(AllyCharacter)) return;
			
			if (AllyCharacter->IsEnemyVisibleInCamera(CurrentEnemyTeam, EnemyState, EnemyImage))
				AllyCharacter->Server_OnEnemySpotted(CurrentEnemyTeam, EnemyState.Get());
		}
	}
}

UImage* UOccupationOverlayMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
{
	UImage* PlayerImage = NewObject<UImage>(this);
	UCanvasPanelSlot* PanelSlot = ParentPanel->AddChildToCanvas(PlayerImage);
	
	if (!IsValid(PlayerImage) || !IsValid(PanelSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("PanelSlot or PlayerImage is null."));
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
		if (NewTeam == ETeam::Pro)
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
	for (const auto Player : OccupationPlayersByMinimap[Team])
	{
		const auto State = Player.Key;
		const auto Image = Player.Value;

		if (!State.IsValid() || !Image.IsValid()) return;

		//플레이어 스테이트가 존재한다고 해서 반드시 폰도 존재하는 것은 아닙니다. 폰도 존재하는지 검사할 필요가 있습니다
		if(!IsValid(State->GetPawn())) return;
		
		FVector2D PlayerPosition(State->GetPawn()->GetActorLocation().X, State->GetPawn()->GetActorLocation().Y);
		const FVector2D NewPlayerPosition = ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);

		if (Image->GetVisibility() == ESlateVisibility::Hidden)
			Image->SetVisibility(ESlateVisibility::Visible);
		
		// 자기 자신을 검사해서, 자신의 위치를 토대로 미니맵의 회전과 위치를 업데이트시켜줍니다.
		if (State == GetOwningPlayerState())
		{
			UpdateMinimapImagePositionAndRotation(*State, NewPlayerPosition);

			const auto PlayerCharacter = State->GetPlayerController()->GetCharacter();
			if (!IsValid(PlayerCharacter)) return;
			const auto LakayaCharacter = Cast<ALakayaBaseCharacter>(PlayerCharacter);
			if (!IsValid(LakayaCharacter) || !IsValid(LakayaCharacter->GetCamera())) return;
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
				if (LakayaCharacter->GetAliveState() == false)
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
	for (const auto Player : OccupationPlayersByMinimap[CurrentEnemyTeam])
	{
		const auto State = Player.Key;
		const auto Image = Player.Value;

		if (!State.IsValid() || !Image.IsValid()) return;

		//플레이어 스테이트가 존재한다고 해서 반드시 폰도 존재하는 것은 아닙니다. 폰도 존재하는지 검사할 필요가 있습니다
		if(!IsValid(State->GetPawn())) return;
		
		// 아군들을 검사해서 아군(나 자신 포함)이 죽어있다면, 죽음 아이콘으로 변경해줍니다.
		if (const auto PlayerCharacter = State->GetPawn())
		{
			if (const ALakayaBaseCharacter* LakayaCharacter = Cast<ALakayaBaseCharacter>(PlayerCharacter))
			{
				// 검사한 아군(나 자신 포함)이 죽었다면 텍스처를 DeathIcon으로 변경해줍니다.
				if (LakayaCharacter->GetAliveState() == false)
					Image->SetBrushFromTexture(DeathIcon);
			}
		}
	}
}

void UOccupationOverlayMinimapWidget::UpdatePlayerPosition(const ETeam& NewTeam,
                                                       const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState)
{
	if(!NewPlayerState.IsValid() || !IsValid(NewPlayerState->GetPawn())) return;
	if (const TWeakObjectPtr<ALakayaBasePlayerState> WeakNewPlayerState = NewPlayerState; !OccupationPlayersByMinimap[NewTeam].Contains(WeakNewPlayerState)) return;
	const auto EnemyImage = OccupationPlayersByMinimap[NewTeam][NewPlayerState].Get();
	if (!IsValid(EnemyImage)) return;
	
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
		EnemyImage->SetBrushFromTexture(AntiIcon);
	else
		EnemyImage->SetBrushFromTexture(ProIcon);
	
	EnemyImage->SetRenderTranslation(NewPlayerPosition + WidgetOffset);

	SetQuestionImage(NewPlayerState);
}

void UOccupationOverlayMinimapWidget::UpdateAreaImageRotation()
{
	if(const auto PlayerCharacter = Cast<ALakayaBaseCharacter>(GetOwningPlayerPawn()))
	{
		if (!IsValid(AntiAreaImage) || !IsValid(CenterAreaImage) || !IsValid(ProAreaImage)) return;
		if (!IsValid(PlayerCharacter->GetCamera())) return;
		
		const FRotator PlayerRotation = PlayerCharacter->GetCamera()->GetComponentRotation();
		
		AntiAreaImage->SetRenderTransformAngle(PlayerRotation.Yaw + 90.0f);
		CenterAreaImage->SetRenderTransformAngle(PlayerRotation.Yaw + 90.0f);
		ProAreaImage->SetRenderTransformAngle(PlayerRotation.Yaw + 90.0f);
	}
}

void UOccupationOverlayMinimapWidget::SetEnemyImage()
{
	for (const auto Enemy : OccupationPlayersByMinimap[CurrentEnemyTeam])
	{
		const auto EnemyImage = Enemy.Value;
		if (!EnemyImage.IsValid()) return;
		EnemyImage->SetBrushFromTexture(QuestionMarkIcon);
	}

	FTimerHandle OldTimerHandle;
	
	GetWorld()->GetTimerManager().SetTimer(OldTimerHandle, FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		for (const auto Enemy : OccupationPlayersByMinimap[CurrentEnemyTeam])
		{
			if (const auto EnemyImage = Enemy.Value; EnemyImage.IsValid())
				EnemyImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}), 3.0f, false);
}