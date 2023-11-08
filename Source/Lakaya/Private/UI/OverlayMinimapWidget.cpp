// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OverlayMinimapWidget.h"

#include "Camera/CameraComponent.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/RetainerBox.h"

void UOverlayMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ParentPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("RetainerCanvasPanel")));
	RetainerBox = Cast<URetainerBox>(GetWidgetFromName(TEXT("RetainerBox_74")));

	MinimapSize = FVector2D(250.0f, 381.0f);
	WidgetOffset = FVector2D(125.0f, 127.5f);
}

void UOverlayMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	UpdateAreaImageRotation();
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
	if(const auto PlayerCharacter = Cast<ALakayaBaseCharacter>(GetOwningPlayerPawn()))
	{
		const FRotator PlayerRotation = PlayerCharacter->GetCamera()->GetComponentRotation();
		EnemyImage->SetRenderTransformAngle((PlayerRotation.Yaw + 90.0f));
	}
}

void UOverlayMinimapWidget::UpdateAreaImageRotation()
{
		const auto PlayerCharacter = Cast<ALakayaBaseCharacter>(GetOwningPlayerPawn());
		const FRotator PlayerRotation = PlayerCharacter->GetCamera()->GetComponentRotation();
	
		AntiAreaImage->SetRenderTransformAngle(PlayerRotation.Yaw + 90.0f);
		CenterAreaImage->SetRenderTransformAngle(PlayerRotation.Yaw + 90.0f);
		ProAreaImage->SetRenderTransformAngle(PlayerRotation.Yaw + 90.0f);
}