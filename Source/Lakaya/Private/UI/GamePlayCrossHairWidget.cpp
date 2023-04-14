// Fill out your copyright notice in the Description page of Project Settings.

#define DO_CHECK 1

#include "UI/GamePlayCrosshairWidget.h"
#include "Components/Image.h"

void UGamePlayCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 후 널체크
#pragma region InitAndNullCheck

	CrosshairImage = Cast<UImage>(GetWidgetFromName(TEXT("Crosshair_Img")));
	check(CrosshairImage != nullptr);

#pragma endregion

	AOccupationGameState* OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GamePlayCrossHairWidget_GameMode is null."));
		return;
	}

	OccupationGameState->OnOccupationChangeGameState.AddUObject(this, &UGamePlayCrosshairWidget::SetGamePlayCrosshairWidget);

	SetVisibility(ESlateVisibility::Hidden);
}

void UGamePlayCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGamePlayCrosshairWidget::SetGamePlayCrosshairWidget(EOccupationGameState ChangeGameState)
{
	if (ChangeGameState == EOccupationGameState::Progress)
	{
		SetVisibility(ESlateVisibility::Visible);
		return;
	}
}