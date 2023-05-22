#define DO_CHECK 1

#include "UI/ScoreBoardElement.h"
#include "Components/TextBlock.h"

void UScoreBoardElement::NativeConstruct()
{
	Super::NativeConstruct();

	//초기화 후 널체크
#pragma region InitAndNullCheck

	PlayerNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("PlayerName_Text")));
	ScoreText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Score_Text")));
	CaptureText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Captrue_Text")));
	KillText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Kill_Text")));
	DeathText = Cast<UTextBlock>(GetWidgetFromName(TEXT("Death_Text")));

	Anti_BackGround_Image = Cast<UImage>(GetWidgetFromName("Anti_BackGround_Image"));
	Pro_BackGround_Image = Cast<UImage>(GetWidgetFromName("Pro_BackGround_Image"));
	Character_Rena_Image = Cast<UImage>(GetWidgetFromName("Character_Rena_Image"));
	Character_Wazi_Image = Cast<UImage>(GetWidgetFromName("Character_Wazi_Image"));
	
	check(PlayerNameText != nullptr);
	check(ScoreText != nullptr);
	check(KillText != nullptr);
	check(DeathText != nullptr);

	check(Anti_BackGround_Image != nullptr);
	check(Pro_BackGround_Image != nullptr);
	check(Character_Rena_Image != nullptr);
	check(Character_Wazi_Image != nullptr);
	
#pragma endregion
}

void UScoreBoardElement::SetData(int8 Rank, const FText& PlayerName, int8 Score) const
{
	// RankText->SetText(FText::AsNumber(Rank));
	// ScoreText->SetText(FText::AsNumber(Score));
	PlayerNameText->SetText(PlayerName);
}

void UScoreBoardElement::SetPlayerName(const FText& PlayerName) const
{
	if (PlayerNameText.IsValid()) PlayerNameText->SetText(PlayerName);
}

void UScoreBoardElement::SetPlayerName(const FString& PlayerName) const
{
	SetPlayerName(FText::FromString(PlayerName));
}

void UScoreBoardElement::SetScoreCount(const uint32& ScoreCount) const
{
	if (ScoreText.IsValid()) ScoreText->SetText(FText::AsNumber(ScoreCount));
}

void UScoreBoardElement::SetCaptureCount(const uint16& CaptureCount) const
{
	if (CaptureText.IsValid()) CaptureText->SetText(FText::AsNumber(CaptureCount));
}

void UScoreBoardElement::SetKillCount(const uint16& KillCount) const
{
	if (KillText.IsValid()) KillText->SetText(FText::AsNumber(KillCount));
}

void UScoreBoardElement::SetDeathCount(const uint16& DeathCount) const
{
	if (DeathText.IsValid()) DeathText->SetText(FText::AsNumber(DeathCount));
}
