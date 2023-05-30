// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/IndividualWidget/IndividualBaseWidget.h"
#include "IndividualLiveScoreElement.generated.h"

/**
 * 
 */
class UImage;
class UTextBlock;
class APlayerState;

UCLASS()
class LAKAYA_API UIndividualLiveScoreElement : public UIndividualBaseWidget
{
	GENERATED_BODY()

public:
	void SetLiveScore(const FString& PlayerName, int32 PlayerRank);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	UImage* RankBox_Img;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerRank_Text;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock*  PlayerRankName_Text;
	
};
