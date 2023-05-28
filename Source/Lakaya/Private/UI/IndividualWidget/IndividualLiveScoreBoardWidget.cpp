// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IndividualWidget/IndividualLiveScoreBoardWidget.h"

#include "Components/TextBlock.h"

void UIndividualLiveScoreBoardWidget::SetScoreBoardPlayerName(const TArray<FPlayerData>& PlayerDataArray)
{
	
}

void UIndividualLiveScoreBoardWidget::SetScoreBoardPlayerAIName(const TArray<FPlayerAIData>& PlayerAIDataArray)
{
	TArray<FPlayerAIData> SortedPlayerAIDataArray = PlayerAIDataArray;
	SortedPlayerAIDataArray.Sort([](const FPlayerAIData& A, const FPlayerAIData& B) {
		return A.KillCount > B.KillCount;
	});
	
	for (int32 i = 0; i < SortedPlayerAIDataArray.Num(); i++)
	{
		const FPlayerAIData& PlayerAIData = SortedPlayerAIDataArray[i];

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
			NameTextBlock->SetText(FText::FromString(PlayerAIData.PlayerName));
			KillTextBlock->SetText(FText::AsNumber(PlayerAIData.KillCount));
		}
	}
}
