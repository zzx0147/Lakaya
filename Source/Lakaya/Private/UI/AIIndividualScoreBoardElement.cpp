// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AIIndividualScoreBoardElement.h"
#include "Components/TextBlock.h"

void UAIIndividualScoreBoardElement::NativeConstruct()
{
	Super::NativeConstruct();

#pragma region InitAndNullCheck
	IndividualRankText = Cast<UTextBlock>(GetWidgetFromName(TEXT("IndividualRank_Text")));
	IndividualPlayerNameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("IndividualPlayerName_Text")));
	IndividualKillText = Cast<UTextBlock>(GetWidgetFromName(TEXT("IndividualKill_Text")));
	IndividualDeathText = Cast<UTextBlock>(GetWidgetFromName(TEXT("IndividualDeath_Text")));
	Individual_BackGround_Image = Cast<UImage>(GetWidgetFromName("Individual_BackGround_Image"));

	check(IndividualPlayerNameText != nullptr);
	check(IndividualKillText != nullptr);
	check(IndividualDeathText != nullptr);

	check(Individual_BackGround_Image != nullptr);
#pragma endregion
}

void UAIIndividualScoreBoardElement::SetIndividualRank(int32 RankCount)
{
	if (IndividualRankText.IsValid()) IndividualRankText->SetText(FText::AsNumber(RankCount));
}

void UAIIndividualScoreBoardElement::SetPlayerName(const FText& PlayerName) const
{
	if (IndividualPlayerNameText.IsValid()) IndividualPlayerNameText->SetText(PlayerName);
}

void UAIIndividualScoreBoardElement::SetPlayerName(const FString& PlayerName) const
{
	SetPlayerName(FText::FromString(PlayerName));
}

void UAIIndividualScoreBoardElement::SetKillCount(const uint16& KillCount) const
{
	IndividualKillCount = KillCount;

	if (IndividualKillText.IsValid()) IndividualKillText->SetText(FText::AsNumber(KillCount));
}

void UAIIndividualScoreBoardElement::SetDeathCount(const uint16& DeathCount) const
{
	if (IndividualDeathText.IsValid()) IndividualDeathText->SetText(FText::AsNumber(DeathCount));
}