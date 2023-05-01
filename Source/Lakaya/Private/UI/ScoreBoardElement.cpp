#define DO_CHECK 1

#include "UI/ScoreBoardElement.h"
#include "Components/TextBlock.h"

void UScoreBoardElement::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 후 널체크
#pragma region InitAndNullCheck

	//TODO: 킬, 데스 텍스트 블럭을 가져옵니다.
	RankText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Rank_Text")));
	PlayerNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("PlayerName_Text")));
	ScoreText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Score_Text")));

	check(RankText != nullptr);
	check(PlayerNameText != nullptr);
	check(ScoreText != nullptr);

#pragma endregion

}

void UScoreBoardElement::SetData(int8 Rank, const FText& PlayerName, int8 Score)
{
	RankText->SetText(FText::AsNumber(Rank));
	PlayerNameText->SetText(PlayerName);
	ScoreText->SetText(FText::AsNumber(Score));
}

void UScoreBoardElement::SetKillCount(const uint16& KillCount)
{
	if (KillText.IsValid()) KillText->SetText(FText::AsNumber(KillCount));
}

void UScoreBoardElement::SetDeathCount(const uint16& DeathCount)
{
	if (DeathText.IsValid()) DeathText->SetText(FText::AsNumber(DeathCount));
}

void UScoreBoardElement::SetPlayerName(const FText& PlayerName)
{
	if (PlayerNameText.IsValid()) PlayerNameText->SetText(PlayerName);
}
