// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameTimeWidget.h"

#include "GameMode/OccupationGameState.h"

UGameTimeWidget::UGameTimeWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TimeTextFormat = FText::FromString(TEXT("{0}:{1}"));
}

void UGameTimeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 바인딩
	OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState.IsStale()) UE_LOG(LogTemp, Error, TEXT("GameTimeWidget_GameState is null."));
	OccupationGameState->OnOccupationChangeGameState.AddUObject(this, &UGameTimeWidget::SetGameTimeWidget);

	GameTimeWidgetText = Cast<UTextBlock>(GetWidgetFromName(TEXT("GameTimeWidgetText")));
	if (GameTimeWidgetText == nullptr) UE_LOG(LogTemp, Warning, TEXT("GameTimeWidgetText is null."));

	SetVisibility(ESlateVisibility::Hidden);
}

void UGameTimeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (OccupationGameState.IsValid())
	{
		auto RemainSeconds = FMath::RoundToInt(OccupationGameState->GetRemainMatchTime());
		GameTimeWidgetText->SetText(FText::Format(TimeTextFormat, RemainSeconds / 60, RemainSeconds % 60));
	}
	else UE_LOG(LogTemp, Warning, TEXT("GameState was nullptr"));
}

void UGameTimeWidget::SetGameTimeWidget(EOccupationGameState ChangeGameState)
{
	if (ChangeGameState == EOccupationGameState::Progress) SetVisibility(ESlateVisibility::Visible);
}
