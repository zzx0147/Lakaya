#include "UI/TeamScoreWidget.h"
#include "Components/TextBlock.h"
#include "GameMode/OccupationGameState.h"

//TODO: AOccupationGameState에 대한 의존성을 제거하면 좋을 것 같습니다.
void UTeamScoreWidget::NativeConstruct()
{
	Super::NativeConstruct();

	const auto OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameTimeWidget_GameState is null."));
		return;
	}

	// 최대 점수를 지정해줍니다.
	MaxScore = OccupationGameState->GetMaxScore();

	// 생성되었을 때, 점수를 초기화 해줍니다.
	OnChangeAntiTeamScore(OccupationGameState->GetTeamScore(ETeam::Anti));
	OnChangeProTeamScore(OccupationGameState->GetTeamScore(ETeam::Pro));

	// 점수가 바뀌게 되면, 점수를 최신화 해줍니다.
	OccupationGameState->OnTeamScoreSignature.AddUObject(this, &UTeamScoreWidget::OnTeamScoreChanged);
}

void UTeamScoreWidget::OnChangeAntiTeamScore(const float& NewScore) const
{
	//TODO: 텍스트 포맷을 따로 저장해두면 더 빠른 퍼포먼스를 기대할 수 있습니다.
	AntiTeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), NewScore)));
	if (NewScore >= MaxScore) AntiTeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), 100.0f)));
}

void UTeamScoreWidget::OnChangeProTeamScore(const float& NewScore) const
{
	ProTeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), NewScore)));
	if (NewScore >= MaxScore) ProTeamScoreText->SetText(FText::FromString(FString::Printf(TEXT("%.1f%%"), 100.0f)));
}

void UTeamScoreWidget::OnTeamScoreChanged(const ETeam& Team, const float& Score) const
{
	if (Team == ETeam::Anti) OnChangeAntiTeamScore(Score);
	else if (Team == ETeam::Pro) OnChangeProTeamScore(Score);
	else UE_LOG(LogScript, Warning, TEXT("Event broadcasted with invalid value! it was %d"), Team);
}