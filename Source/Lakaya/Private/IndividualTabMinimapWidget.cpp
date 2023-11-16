// Fill out your copyright notice in the Description page of Project Settings.


#include "IndividualTabMinimapWidget.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/CanvasPanelSlot.h"
#include "Camera/CameraComponent.h"
#include "GameMode/AIIndividualGameState.h"

void UIndividualTabMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const TWeakObjectPtr<ALakayaBasePlayerState> MyPlayerState = Cast<ALakayaBasePlayerState>(GetOwningPlayerState());
	if (MyPlayerState == nullptr)
		UE_LOG(LogTemp, Warning, TEXT("MyPlayerState is null."));
	
	// 자기 자신의 위치를 업데이트 해줍니다.
	UpdatePlayerPosition(MyPlayerState);

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
		if (!PlayerState.IsValid() || !PlayerImage.IsValid()) return;
		
		ALakayaBaseCharacter* MyPlayerCharacter = Cast<ALakayaBaseCharacter>(MyPlayerState->GetPawn());
		if (!IsValid(MyPlayerCharacter)) return;
		if (MyPlayerCharacter->IsEnemyVisibleInCamera(ETeam::Individual, PlayerState, PlayerImage))
		{
			// 해당 적이 나의 시야에 있다면 해당 적을 미니맵에 업데이트 해줍니다.
			UpdatePlayerPosition(PlayerState);
		}
	}
}

void UIndividualTabMinimapWidget::UpdatePlayerPosition(const TWeakObjectPtr<ALakayaBasePlayerState>& NewPlayerState)
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
	const FVector2D NewPlayerPosition = const_cast<UIndividualTabMinimapWidget*>(this)->ConvertWorldToMiniMapCoordinates(PlayerPosition, MinimapSize);

	if (NewPlayerImage->GetVisibility() == ESlateVisibility::Hidden)
		NewPlayerImage->SetVisibility(ESlateVisibility::Visible);

	ALakayaBaseCharacter* LakayaBaseCharacter = Cast<ALakayaBaseCharacter>(NewPlayerState->GetPawn());
	if (!IsValid(LakayaBaseCharacter)) return;
	if (!LakayaBaseCharacter->GetAliveState())
	{
		NewPlayerImage->SetBrushFromTexture(DeathIcon);

		if (NewPlayerState == GetOwningPlayerState())
		{
			NewPlayerImage->SetRenderTransformAngle(90.0f);
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

			GetWorld()->GetTimerManager().SetTimer(NewTimerHandle, FTimerDelegate::CreateWeakLambda(this, [this, NewPlayerImage]()
			{
				NewPlayerImage->SetBrushFromTexture(QuestionMarkIcon);
			}), 0.1f, false);
		}
		else if (NewPlayerState == GetOwningPlayerState())
		{
			const auto PlayerCharacter = NewPlayerState->GetPlayerController()->GetCharacter();
			if (!IsValid(PlayerCharacter)) return;
			const auto LakayaCharacter = Cast<ALakayaBaseCharacter>(PlayerCharacter);
			if (!IsValid(LakayaCharacter)) return;
			const FRotator PlayerRotation = LakayaCharacter->GetCamera()->GetComponentRotation();

			NewPlayerImage->SetRenderTransformAngle(PlayerRotation.Yaw + 90.0f);
			NewPlayerImage->SetBrushFromTexture(IndividualOwnIcon);
		}
	}

	NewPlayerImage->SetRenderTranslation(NewPlayerPosition + WidgetOffset + FVector2D(-275.0f, 0));

	SetQuestionImage(NewPlayerState);
}

void UIndividualTabMinimapWidget::SetEnemyImage()
{
	for (const auto& Enemy : IndividualPlayersByMinimap)
	{
		const auto& EnemyImage = Enemy.Value;
		if (!EnemyImage.IsValid()) return;
		EnemyImage->SetBrushFromTexture(QuestionMarkIcon);
	}

	FTimerHandle OldTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(OldTimerHandle, FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		for (const auto& Enemy : IndividualPlayersByMinimap)
		{
			const auto& EnemyImage = Enemy.Value;
			if (!EnemyImage.IsValid()) return;
			EnemyImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}), 3.0f, false);
}

UImage* UIndividualTabMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
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
}
