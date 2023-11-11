// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IndividualOverlayMinimapWidget.h"

#include "Camera/CameraComponent.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/CanvasPanelSlot.h"
#include "GameMode/AIIndividualGameState.h"

void UIndividualOverlayMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UIndividualOverlayMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const TWeakObjectPtr<ALakayaBasePlayerState> MyPlayerState = Cast<ALakayaBasePlayerState>(GetOwningPlayerState());

	// 자기 자신의 위치를 업데이트 해줍니다.
	UpdatePlayerPosition(MyPlayerState);

	// 미니맵 상에 표시되는 적들의 아이콘 회전 값을 업데이트 해줍니다.
	for (const auto& Player : IndividualPlayersByMinimap)
	{
		UpdateEnemyImageRotation(Player.Value);
	}
	
	// 만약 와지(본인)이 궁극기를 사용중이라면, 적들의 위치를 업데이트 합니다.
	if (const auto& IndividualGameState = Cast<AAIIndividualGameState>(GetWorld()->GetGameState()))
	{
		if (IndividualGameState->GetbIsClairvoyanceActivated())
		{
			Super::UpdatePlayerPosition(CurrentTeam);
			return;
		}
	}
	
	for (const auto& Player : IndividualPlayersByMinimap)
	{
		const auto& PlayerState = Player.Key;
		const auto& PlayerImage = Player.Value;
		
		ALakayaBaseCharacter* MyPlayerCharacter = Cast<ALakayaBaseCharacter>(MyPlayerState->GetPawn());
		if (MyPlayerCharacter->IsEnemyVisibleInCamera(ETeam::Individual, PlayerState, PlayerImage))
		{
			// 해당 적이 나의 시야에 있다면 해당 적을 미니맵에 업데이트 해줍니다.
			UpdatePlayerPosition(PlayerState);
		}
	}
}

UImage* UIndividualOverlayMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
{
	UE_LOG(LogTemp, Warning, TEXT("CreatePlayerImage"));
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
	{
		PlayerImage->SetBrushFromTexture(IndividualOwnIcon);
		return PlayerImage;
	}
	else
	{
		PlayerImage->SetBrushFromTexture(IndividualEnemyIcon);
		return PlayerImage;
	}
	
	return nullptr;
}

void UIndividualOverlayMinimapWidget::UpdatePlayerPosition(const TWeakObjectPtr<ALakayaBasePlayerState>& NewPlayerState)
{
#pragma region Null Check
	if (const TWeakObjectPtr<ALakayaBasePlayerState> WeakNewPlayerState = NewPlayerState;
		!IndividualPlayersByMinimap.Contains(WeakNewPlayerState))
	{
		UE_LOG(LogTemp, Warning, TEXT("WeakNewPlayerState is not in IndividualPlayersByMinimap."));
		return;
	}
	
	const auto& NewPlayerImage = IndividualPlayersByMinimap[NewPlayerState];
	if (NewPlayerImage == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("NewPlayerImage is null."));
		return;
	}
#pragma endregion

	const FVector2D PlayerPosition(NewPlayerState->GetPawn()->GetActorLocation().X, NewPlayerState->GetPawn()->GetActorLocation().Y);
	const FVector2D NewPlayerPosition = const_cast<UIndividualOverlayMinimapWidget*>(this)->ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);

	if (NewPlayerImage->GetVisibility() == ESlateVisibility::Hidden)
		NewPlayerImage->SetVisibility(ESlateVisibility::Visible);

	const ALakayaBaseCharacter* LakayaBaseCharacter = Cast<ALakayaBaseCharacter>(NewPlayerState->GetPawn());
	if (!LakayaBaseCharacter->GetAliveState())
	{
		NewPlayerImage->SetBrushFromTexture(DeathIcon);

		if (NewPlayerState == GetOwningPlayerState())
		{
			const auto PlayerCharacter = NewPlayerState->GetPlayerController()->GetCharacter();
			const auto LakayaCharacter = Cast<ALakayaBaseCharacter>(PlayerCharacter);
			const FRotator PlayerRotation = LakayaCharacter->GetCamera()->GetComponentRotation();

			NewPlayerImage->SetRenderTransformAngle(PlayerRotation.Yaw + 90.0f);
			UpdateMinimapImagePositionAndRotation(*NewPlayerState, NewPlayerPosition);
		}
	}
	else
	{
		if (NewPlayerState != GetOwningPlayerState())
		{
			NewPlayerImage->SetBrushFromTexture(IndividualEnemyIcon);
			FTimerHandle NewTimerHandle;
			if (PlayerTimers.Contains(NewPlayerState))
				GetWorld()->GetTimerManager().ClearTimer(PlayerTimers[NewPlayerState]);

			GetWorld()->GetTimerManager().SetTimer(NewTimerHandle, [this, NewPlayerImage]()
			{
				NewPlayerImage->SetBrushFromTexture(QuestionMarkIcon);
			}, 0.1f, false);
		}
		else if (NewPlayerState == GetOwningPlayerState())
		{
			const auto PlayerCharacter = NewPlayerState->GetPlayerController()->GetCharacter();
			const auto LakayaCharacter = Cast<ALakayaBaseCharacter>(PlayerCharacter);
			const FRotator PlayerRotation = LakayaCharacter->GetCamera()->GetComponentRotation();

			NewPlayerImage->SetRenderTransformAngle(PlayerRotation.Yaw + 90.0f);
			NewPlayerImage->SetBrushFromTexture(IndividualOwnIcon);
			UpdateMinimapImagePositionAndRotation(*NewPlayerState, NewPlayerPosition);
		}
	}

	NewPlayerImage->SetRenderTranslation(NewPlayerPosition + WidgetOffset);

	SetQuestionImage(NewPlayerState);
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

void UIndividualOverlayMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	Super::UpdatePlayerPosition(Team);
}

void UIndividualOverlayMinimapWidget::UpdatePlayerPosition(const ETeam& NewTeam,
	const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState)
{
	Super::UpdatePlayerPosition(NewTeam, NewPlayerState);
}

void UIndividualOverlayMinimapWidget::SetEnemyImage()
{
	for (const auto& Enemy : IndividualPlayersByMinimap)
	{
		const auto& EnemyImage = Enemy.Value;
		EnemyImage->SetBrushFromTexture(QuestionMarkIcon);
	}

	FTimerHandle OldTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(OldTimerHandle, [this]()
	{
		for (const auto& Enemy : IndividualPlayersByMinimap)
		{
			const auto& EnemyImage = Enemy.Value;
			EnemyImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}, 3.0f, false);
}
