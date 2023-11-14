// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IndividualWidget/IndividualLiveScoreElement.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UIndividualLiveScoreElement::NativeConstruct()
{
	Super::NativeConstruct();

#pragma region InitAndNullCheck
	RankBox_Img = Cast<UImage>(GetWidgetFromName("RankBox_Img"));
	PlayerCheck_Img = Cast<UImage>(GetWidgetFromName("PlayerCheck_Img"));
	NameText = Cast<UTextBlock>(GetWidgetFromName(TEXT("NameText")));
	RankText = Cast<UTextBlock>(GetWidgetFromName(TEXT("RankText")));
	KillText = Cast<UTextBlock>(GetWidgetFromName(TEXT("KillText")));

	check(NameText != nullptr);
	check(RankText != nullptr);
	check(KillText != nullptr);
	
	check(RankBox_Img != nullptr);
	check(PlayerCheck_Img != nullptr);
#pragma endregion
}

void UIndividualLiveScoreElement::Set1stElementBoxImage()
{
	if (RankBox_Img.IsValid())
	{
		FString ImagePath = TEXT("/Game/UI_2/UI_IngameIndividual/T_InGame_Individual_Rank_Box"); // 이미지 경로
		UTexture2D* LoadedTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *ImagePath));
    
		if (LoadedTexture)
		{
			RankBox_Img->SetBrushFromTexture(LoadedTexture);
		}
	}
}

void UIndividualLiveScoreElement::SetOtherElementBoxImage()
{
	if (RankBox_Img.IsValid())
	{
		FString ImagePath = TEXT("/Game/UI_2/UI_IngameIndividual/T_InGame_Individual_Rank_Box_2"); // 이미지 경로
		UTexture2D* LoadedTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *ImagePath));
    
		if (LoadedTexture)
		{
			RankBox_Img->SetBrushFromTexture(LoadedTexture);
		}
	}
}

void UIndividualLiveScoreElement::SetPlayerCheckVisible()
{
	PlayerCheck_Img->SetVisibility(ESlateVisibility::Visible);
}

void UIndividualLiveScoreElement::SetPlayerCheckHidden()
{
	PlayerCheck_Img->SetVisibility(ESlateVisibility::Hidden);
}

void UIndividualLiveScoreElement::SetIndividualRank(int32 RankCount)
{
	if (RankText.IsValid())	RankText->SetText(FText::AsNumber(RankCount));
}

void UIndividualLiveScoreElement::SetPlayerName(const FText& PlayerName) const
{
	if (NameText.IsValid()) NameText->SetText(PlayerName);
}

void UIndividualLiveScoreElement::SetPlayerName(const FString& PlayerName) const
{
	SetPlayerName(FText::FromString(PlayerName));
}

void UIndividualLiveScoreElement::SetKillCount(const uint16& KillCount) const
{
	IndividualKillCount = KillCount;
	
	if (KillText.IsValid())	KillText->SetText(FText::AsNumber(KillCount));
}
