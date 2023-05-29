// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IndividualWidget/IndividualLiveScoreBoardWidget.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "UI/IndividualWidget/IndividualLiveScoreElement.h"

void UIndividualLiveScoreBoardWidget::SetScoreBoardPlayerName(const TArray<FPlayerData>& PlayerDataArray)
{
}

void UIndividualLiveScoreBoardWidget::SetScoreBoardPlayerAIName(const TArray<FPlayerAIData>& PlayerAIDataArray)
{
	// TArray<FPlayerAIData> SortedPlayerAIDataArray = PlayerAIDataArray;
	// SortedPlayerAIDataArray.Sort([](const FPlayerAIData& A, const FPlayerAIData& B) {
	// 	return A.KillCount > B.KillCount;
	// });
	//
	// for (int32 i = 0; i < SortedPlayerAIDataArray.Num(); i++)
	// {
	// 	const FPlayerAIData& PlayerAIData = SortedPlayerAIDataArray[i];
	//
	// 	UTextBlock* NameTextBlock = nullptr;
	// 	switch (i)
	// 	{
	// 	case 0:
	// 		NameTextBlock = Score1stNameTextBlock;
	// 		break;
	// 	case 1:
	// 		NameTextBlock = Score2ndNameTextBlock;
	// 		break;
	// 	case 2:
	// 		NameTextBlock = Score3rdNameTextBlock;
	// 		break;
	// 	case 3:
	// 		NameTextBlock = Score4thNameTextBlock;
	// 		break;
	// 	case 4:
	// 		NameTextBlock = Score5thNameTextBlock;
	// 		break;
	// 	case 5:
	// 		NameTextBlock = Score6thNameTextBlock;
	// 		break;
	// 	}
	//
	// 	if (NameTextBlock)
	// 		NameTextBlock->SetText(FText::FromString(PlayerAIData.PlayerName));
	// }
}