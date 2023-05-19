#include "UI/TeamScoreWidget.h"
#include "Components/TextBlock.h"
#include "GameMode/OccupationGameState.h"

void UTeamScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	auto OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
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

	// 최대 점수를 지정해줍니다.
	MaxScore = OccupationGameState->GetMaxScore();

	// 생성되었을 때, 점수를 초기화 해줍니다.
	OnChangeATeamScore(OccupationGameState->GetTeamScore(EPlayerTeam::A));
	OnChangeBTeamScore(OccupationGameState->GetTeamScore(EPlayerTeam::B));

	// 점수가 바뀌게 되면, 점수를 최신화 해줍니다.
	OccupationGameState->OnTeamScoreSignature.AddUObject(this, &UTeamScoreWidget::OnTeamScoreChanged);

	// if (OccupationGameState->OnTeamScoreSignature.IsBound())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("OnTeamScoreSignature is bound!"));
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("OnTeamScoreSignature is not bound!"));
	// }
}

void UTeamScoreWidget::OnChangeATeamScore(const float& NewScore) const
{
	if (GEngine->GetNetMode(GetWorld()) == ENetMode::NM_ListenServer)
	{
		ATeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), NewScore)));
		if (NewScore >= MaxScore) ATeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), 1.0f)));
	}
	else
	{
		ATeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), NewScore)));
		if (NewScore >= MaxScore) ATeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), 1.0f)));
	}
}

void UTeamScoreWidget::OnChangeBTeamScore(const float& NewScore) const
{
	BTeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), NewScore)));
	if (NewScore >= MaxScore) BTeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), 1.0f)));
}

void UTeamScoreWidget::OnTeamScoreChanged(const EPlayerTeam& Team, const float& Score) const
{
	if (Team == EPlayerTeam::A) OnChangeATeamScore(Score);
	else if (Team == EPlayerTeam::B) OnChangeBTeamScore(Score);
	else UE_LOG(LogScript, Warning, TEXT("Event broadcasted with invalid value! it was %d"), Team);
	// UE_LOG(LogTemp, Warning, TEXT("OnTeamScoreChanged."));
}