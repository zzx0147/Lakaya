// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IndividualWidget/IndividualLiveScoreBoardWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "UI/IndividualWidget/IndividualLiveScoreElement.h"

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
		UImage* ImageBox = nullptr;
		UImage* PlayerImageBox = nullptr;
		
		switch (i)
		{
		case 0:
			NameTextBlock = Score1stNameTextBlock;
			PlayerImageBox = Slot_Player_1st;
			break;
		case 1:
			NameTextBlock = Score2ndNameTextBlock;
			ImageBox = Slot_2nd;
			PlayerImageBox = Slot_Player_2nd;
			break;
		case 2:
			NameTextBlock = Score3rdNameTextBlock;
			ImageBox = Slot_3rd;
			PlayerImageBox = Slot_Player_3rd;
			break;
		case 3:
			NameTextBlock = Score4thNameTextBlock;
			ImageBox = Slot_4th;
			PlayerImageBox = Slot_Player_4th;
			break;
		case 4:
			NameTextBlock = Score5thNameTextBlock;
			ImageBox = Slot_5th;
			PlayerImageBox = Slot_Player_5th;
			break;
		case 5:
			NameTextBlock = Score6thNameTextBlock;
			ImageBox = Slot_6th;
			PlayerImageBox = Slot_Player_6th;
			break;
		}
	
		if (NameTextBlock)
			NameTextBlock->SetText(FText::FromString(PlayerAIData.PlayerName));
		if (ImageBox)
		{
			UTexture2D* AISlotImage = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/UI_2/UI_IngameIndividual/T_InGame_Individual_Rank_Box_2.T_InGame_Individual_Rank_Box_2'"));
			ImageBox->SetBrushFromTexture(AISlotImage);
		}
		if (PlayerImageBox)
		{
			if(PlayerAIData.bIsPlayerCheck == true)
			{
				UTexture2D* PlayerLocalSlotImage = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/UI_2/UI_IngameIndividual/T_InGame_Individual_Rank_Cursor.T_InGame_Individual_Rank_Cursor'"));
				PlayerImageBox->SetBrushFromTexture(PlayerLocalSlotImage);
				
				PlayerImageBox->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
			}
			else
			{
				PlayerImageBox->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.0f));
			}
		}
	}
}