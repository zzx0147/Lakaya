// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OverlayMinimapWidget.h"

#include "Camera/CameraComponent.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/RetainerBox.h"

void UOverlayMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RetainerBox = Cast<URetainerBox>(GetWidgetFromName(TEXT("RetainerBox_74")));
}

void UOverlayMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

UImage* UOverlayMinimapWidget::CreatePlayerImage(const ETeam& NewTeam, const bool bMyPlayer)
{
	return Super::CreatePlayerImage(NewTeam, bMyPlayer);
}

FVector2d UOverlayMinimapWidget::ConvertWorldToMiniMapCoordinates(const FVector2D& PlayerLocation,
	const FVector2D& MiniMapSize)
{
	return Super::ConvertWorldToMiniMapCoordinates(PlayerLocation, MiniMapSize);
}

void UOverlayMinimapWidget::UpdatePlayerPosition(const ETeam& Team)
{
	Super::UpdatePlayerPosition(Team);
}

void UOverlayMinimapWidget::UpdatePlayerPosition(const ETeam& NewTeam,
	const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState)
{
	Super::UpdatePlayerPosition(NewTeam, NewPlayerState);
}

void UOverlayMinimapWidget::HidePlayerPosition(const ETeam& NewTeam,
	const TWeakObjectPtr<ALakayaBasePlayerState> NewPlayerState)
{
	Super::HidePlayerPosition(NewTeam, NewPlayerState);
}

void UOverlayMinimapWidget::UpdateMinimapImagePositionAndRotation(const ALakayaBasePlayerState& NewPlayerState,
                                                              const FVector2D NewPosition) const
{
	const auto PlayerCharacter = NewPlayerState.GetPlayerController()->GetCharacter();
	const auto LakayaCharacter = Cast<ALakayaBaseCharacter>(PlayerCharacter);
	const FRotator PlayerRotation = LakayaCharacter->GetCamera()->GetComponentRotation();
	
	ParentPanel->SetRenderTranslation(-NewPosition);
	RetainerBox->SetRenderTransformAngle(-(PlayerRotation.Yaw + 90.0f));
}

void UOverlayMinimapWidget::UpdateEnemyImageRotation(const TWeakObjectPtr<UImage> EnemyImage) const
{
	const auto PlayerCharacter = Cast<ALakayaBaseCharacter>(GetOwningPlayerPawn());
	const FRotator PlayerRotation = PlayerCharacter->GetCamera()->GetComponentRotation();

	EnemyImage->SetRenderTransformAngle((PlayerRotation.Yaw + 90.0f));
}