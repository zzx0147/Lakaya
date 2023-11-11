// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/OccupationTabMinimapWidget.h"

#include "Camera/CameraComponent.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/CanvasPanelSlot.h"
#include "GameMode/OccupationGameState.h"

void UOccupationTabMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TeamIcons.Emplace(ETeam::Anti, AntiIcon);
	TeamIcons.Emplace(ETeam::Pro, ProIcon);
}

void UOccupationTabMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!UpdateMinimap) return;

	// 자신의 팀(자기 자신 포함)위치를 업데이트 해줍니다.
	UpdatePlayerPosition(CurrentTeam);

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

void UOccupationTabMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	Super::UpdatePlayerPosition(Team);

	// 자기 자신을 검사해서, 자신의 위치를 토대로 미니맵을 회전과 위치를 업데이트시켜줍니다.
	for (auto& Player : OccupationPlayersByMinimap[Team])
	{
		const auto& State = Player.Key;
		const auto& Image = Player.Value;

		FVector2D PlayerPosition(State->GetPawn()->GetActorLocation().X, State->GetPawn()->GetActorLocation().Y);
		const FVector2D NewPlayerPosition = ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);
		
		if (Image->GetVisibility() == ESlateVisibility::Hidden)
			Image->SetVisibility(ESlateVisibility::Visible);

		if (State == GetOwningPlayerState())
		{
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

	// 적들의 사망 여부를 체크해서 사망했다면, 사망 아이콘으로 변경해줍니다.
	for (auto& Player : OccupationPlayersByMinimap[CurrentEnemyTeam])
	{
		const auto& State = Player.Key;
		const auto& Image = Player.Value;

		// 아군들을 검사해서 아군(나 자신 포함)이 죽어있다면, 죽음 아이콘으로 변경해줍니다.
		if (const auto PlayerCharacter = State->GetPawn())
		{
			if (const ALakayaBaseCharacter* LakayaCharacter = Cast<ALakayaBaseCharacter>(PlayerCharacter))
			{
				// 검사한 적이 죽었다면 텍스처를 DeathIcon으로 변경해줍니다.
				if (!LakayaCharacter->GetAliveState())
				{
					Image->SetBrushFromTexture(DeathIcon);
				}
				else 
				{
					switch (CurrentEnemyTeam)
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

void UOccupationTabMinimapWidget::UpdatePlayerPosition(const ETeam& NewTeam,
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

	if (NewTeam == ETeam::Anti)
	{
		EnemyImage->SetBrushFromTexture(AntiIcon);
	}
	else
	{
		EnemyImage->SetBrushFromTexture(ProIcon);
	}

	EnemyImage->SetRenderTranslation(NewPlayerPosition + WidgetOffset);

	SetQuestionImage(NewPlayerState);
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