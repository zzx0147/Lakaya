// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IndividualWidget/IndividualLiveScoreElement.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UIndividualLiveScoreElement::SetLiveScore(const FString& PlayerName, int32 PlayerRank)
{
	PlayerRank_Text->SetText(FText::AsNumber(PlayerRank));
	PlayerRankName_Text->SetText(FText::FromString(PlayerName));
}

void UIndividualLiveScoreElement::NativeConstruct()
{
	Super::NativeConstruct();

	// InitializeWidgetPtr(RankBoxImage, TEXT("RankBox_Etc_Img"));
	// InitializeWidgetPtr(PlayerRankTextBlock, TEXT("PlayerRank_Text"));
	// InitializeWidgetPtr(PlayerRankNameTextBlock, TEXT("PlayerRankName_Text"));

}
