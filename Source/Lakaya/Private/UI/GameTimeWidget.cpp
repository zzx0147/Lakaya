// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameTimeWidget.h"

#include "GameMode/OccupationGameState.h"

void UGameTimeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TimeTextFormat = FText::FromString(TEXT("{0}:{1}"));

	OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState.IsStale()) UE_LOG(LogTemp, Error, TEXT("GameTimeWidget_GameState is null."));
	OccupationGameState->OnOccupationChangeGameState.AddUObject(this, &UGameTimeWidget::SetGameTimeWidget);

	// 바인딩
	GameTimeWidgetText = Cast<UTextBlock>(GetWidgetFromName(TEXT("GameTimeWidgetText")));
	if (GameTimeWidgetText == nullptr) UE_LOG(LogTemp, Warning, TEXT("GameTimeWidgetText is null."));
	
	SetVisibility(ESlateVisibility::Hidden);
}

void UGameTimeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (OccupationGameState.IsValid())
	{
		auto TotalSeconds = FMath::RoundToInt(OccupationGameState->GetRemainMatchTime());
		GameTimeWidgetText->SetText(FText::Format(TimeTextFormat, TotalSeconds / 60, TotalSeconds % 60));
	}
	else UE_LOG(LogTemp, Warning, TEXT("GameState was nullptr"));
}

void UGameTimeWidget::SetGameTimeWidget(EOccupationGameState ChangeGameState)
{
	if (ChangeGameState == EOccupationGameState::Progress) SetVisibility(ESlateVisibility::Visible);
}
