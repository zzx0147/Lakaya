// Fill out your copyright notice in the Description page of Project Settings.

#define DO_CHECK 1

#include "UI/GamePlayCrosshairWidget.h"
#include "Components/Image.h"
#include "GameMode/IndividualGameState.h"

void UGamePlayCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 후 널체크
#pragma region InitAndNullCheck

	CrosshairImage = Cast<UImage>(GetWidgetFromName(TEXT("Crosshair_Img")));
	check(CrosshairImage != nullptr);

#pragma endregion

	AIndividualGameState* IndividualGameState = Cast<AIndividualGameState>(GetWorld()->GetGameState());
	if (IndividualGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode is null."));
		return;
	}

	IndividualGameState->OnChangeGameState.AddUObject(this, &UGamePlayCrosshairWidget::SetGamePlayCrosshairWidget);

	SetVisibility(ESlateVisibility::Hidden);
}

void UGamePlayCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGamePlayCrosshairWidget::SetGamePlayCrosshairWidget(EGameState ChangeGameState)
{
	if (ChangeGameState == EGameState::Progress)
	{
		SetVisibility(ESlateVisibility::Visible);
		return;
	}
}