// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TeamScoreWidget.h"
#include "Components/TextBlock.h"
#include "GameMode/OccupationGameState.h"

bool UTeamScoreWidget::OnMatchStart()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	return true;
}

void UTeamScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const auto OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
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

	BTeamScoreText = Cast<UTextBlock>(GetWidgetFromName(TEXT("BTeamScoreText")));
	if (BTeamScoreText == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTeamScoreText is null."));
		return;
	}

	MaxScore = OccupationGameState->GetMaxScore();
	OnChangeATeamScore(OccupationGameState->GetTeamScore(EPlayerTeamState::A));
	OnChangeBTeamScore(OccupationGameState->GetTeamScore(EPlayerTeamState::B));

	OccupationGameState->OnTeamScoreChanged.AddUObject(this, &UTeamScoreWidget::OnTeamScoreChanged);
}

void UTeamScoreWidget::OnChangeATeamScore(const float& NewScore) const
{
	ATeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("A팀 %.1f%%"), NewScore)));
	if (NewScore >= MaxScore) ATeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("A팀 %.1f%%"), 1.0f)));
}

void UTeamScoreWidget::OnChangeBTeamScore(const float& NewScore) const
{
	BTeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("B팀 %.1f%%"), NewScore)));
	if (NewScore >= MaxScore) BTeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("B팀 %.1f%%"), 1.0f)));
}

void UTeamScoreWidget::OnTeamScoreChanged(const EPlayerTeamState& Team, const float& Score) const
{
	if (Team == EPlayerTeamState::A) OnChangeATeamScore(Score);
	else if (Team == EPlayerTeamState::B) OnChangeBTeamScore(Score);
	else UE_LOG(LogScript, Warning, TEXT("Event broadcasted with invalid value! it was %d"), Team);
}
