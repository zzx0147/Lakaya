#include "UI/TeamScoreWidget.h"
#include "Components/TextBlock.h"


UTeamScoreWidget::UTeamScoreWidget(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer), MaxScore(100)
{
	ScoreFormatText = FText::FromString(TEXT("{0}"));
}

void UTeamScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTeamScoreWidget::SetTeamScore(const ETeam& Team, const float& NewScore)
{
	if (Team == ETeam::None || Team == ETeam::Individual) return;

	(Team == ETeam::Anti ? AntiTeamScoreText : ProTeamScoreText)->SetText(FText::Format(ScoreFormatText, FMath::Min(NewScore, MaxScore)));
}

void UTeamScoreWidget::SetMaxScore(const float& ArgMaxScore)
{
	MaxScore = ArgMaxScore;
	MaxScoreText->SetText(FText::Format(ScoreFormatText, MaxScore));
}

void UTeamScoreWidget::SetMaxScoreVisibility(const bool& IsVisible)
{
	MaxScoreText->SetVisibility(IsVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}
