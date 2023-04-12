// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TeamScoreWidget.h"
#include "Components/TextBlock.h"

void UTeamScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameTimeWidget_GameState is null."));
		return;
	}

	ATeamScoreText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ATeamScoreText")));
	if (ATeamScoreText == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATeamScoreText is null."));
		return;
	}

	OnChangeATeamScore(OccupationGameState->GetATeamScore());
	OnChangeBTeamScore(OccupationGameState->GetBTeamScore());
	
	BTeamScoreText = Cast<UTextBlock>(GetWidgetFromName(TEXT("BTeamScoreText")));
	if (BTeamScoreText == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTeamScoreText is null."));
		return;
	}

	OccupationGameState->OnOccupationChangeATeamScore.AddUObject(this, &UTeamScoreWidget::OnChangeATeamScore);
	OccupationGameState->OnOccupationChangeBTeamScore.AddUObject(this, &UTeamScoreWidget::OnChangeBTeamScore);
	OccupationGameState->OnOccupationChangeGameState.AddUObject(this, &UTeamScoreWidget::SetTeamScoreWidget);
	
	SetVisibility(ESlateVisibility::Hidden);
}

void UTeamScoreWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UTeamScoreWidget::SetTeamScoreWidget(EOccupationGameState ChangeGameState)
{
	if (ChangeGameState == EOccupationGameState::Progress)
	{
		SetVisibility(ESlateVisibility::Visible);
		return;
	}
}

void UTeamScoreWidget::ReMoveTeamScoreWidget(EOccupationGameState ChangeGameState)
{
	// TODO
}

void UTeamScoreWidget::OnChangeATeamScore(float NewScore)
{
	ATeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("A팀 %.1f%%"), NewScore)));
	if (NewScore >= OccupationGameState->GetMaxScore())
	{
		ATeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("A팀 %.1f%%"), 1.0f)));
	}
}

void UTeamScoreWidget::OnChangeBTeamScore(float NewScore)
{
	BTeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("B팀 %.1f%%"), NewScore)));
	if (NewScore >= OccupationGameState->GetMaxScore())
	{
		BTeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("B팀 %.1f%%"), 1.0f)));
	}
}