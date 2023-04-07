// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameResultWidget.h"

void UGameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_GameMode is null."));
		return;
	}

	GameResultWidgetText = Cast<UTextBlock>(GetWidgetFromName(TEXT("GameResultWidgetText")));
	if (GameResultWidgetText == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_JoinedPlayerText is null."));
		return;
	}

	OccupationGameState->OnOccupationChangeOccupationWinner.AddUObject(this, &UGameResultWidget::OnChangeWinner);

	SetVisibility(ESlateVisibility::Hidden);
}

void UGameResultWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGameResultWidget::ReMoveLoadingWidget(EOccupationGameState ChangeGameState)
{
	// TODO
}

void UGameResultWidget::OnChangeWinner(EOccupationWinner NewWinner)
{
	SetVisibility(ESlateVisibility::Visible);

	FString WinnerString;
	switch (OccupationGameState->GetOccupationWinner())
	{
	case EOccupationWinner::UnCertain:
		WinnerString = FString(TEXT("Undecided"));
		break;
	case EOccupationWinner::A:
		WinnerString = FString(TEXT("A"));
		break;
	case EOccupationWinner::B:
		WinnerString = FString(TEXT("B"));
		break;
	default:
		WinnerString = FString(TEXT("Unknown"));
		break;
	}
	
	GameResultWidgetText->SetText(FText::FromString(FString::Printf(TEXT("%s팀이 승리하였습니다 !"), *WinnerString)));
}