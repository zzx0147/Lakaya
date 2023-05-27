// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IndividualWidget/AIIndividualScoreBoardWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/Character.h"

UAIIndividualScoreBoardWidget::UAIIndividualScoreBoardWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// static const ConstructorHelpers::FClassFinder<UAIIndividualScoreBoardWidget> ElementFinder(
	// TEXT("/Game/Blueprints/UMG/IndividualWidget/WBP_AIIndividualScoreBoardWidget"));
}

void UAIIndividualScoreBoardWidget::SetScoreBoardPlayerName(const TArray<FPlayerData>& PlayerDataArray)
{
	TArray<FPlayerData> SortedPlayerDataArray = PlayerDataArray;
	SortedPlayerDataArray.Sort([](const FPlayerData& A, const FPlayerData& B) {
		return A.KillCount > B.KillCount;
	});
	
	for (int32 i = 0; i < SortedPlayerDataArray.Num(); i++)
	{
		const FPlayerData& PlayerData = SortedPlayerDataArray[i];

		UTextBlock* NameTextBlock = nullptr;
		UTextBlock* KillTextBlock = nullptr;
		
		switch (i)
		{
		case 0:
			NameTextBlock = Score1stNameTextBlock;
			KillTextBlock = Score1stKillCountTextBlock;
			break;
		case 1:
			NameTextBlock = Score2ndNameTextBlock;
			KillTextBlock = Score2ndKillCountTextBlock;
			break;
		case 2:
			NameTextBlock = Score3rdNameTextBlock;
			KillTextBlock = Score3rdKillCountTextBlock;
			break;
		case 3:
			NameTextBlock = Score4thNameTextBlock;
			KillTextBlock = Score4thKillCountTextBlock;
			break;
		case 4:
			NameTextBlock = Score5thNameTextBlock;
			KillTextBlock = Score5thKillCountTextBlock;
			break;
		case 5:
			NameTextBlock = Score6thNameTextBlock;
			KillTextBlock = Score6thKillCountTextBlock;
			break;
		}

		if (NameTextBlock)
		{
			NameTextBlock->SetText(FText::FromString(PlayerData.PlayerName));
			KillTextBlock->SetText(FText::AsNumber(PlayerData.KillCount));
		}
	}
}
