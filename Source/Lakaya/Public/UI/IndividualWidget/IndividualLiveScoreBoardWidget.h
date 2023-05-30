// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/IndividualWidget/IndividualBaseWidget.h"
#include "IndividualLiveScoreBoardWidget.generated.h"

/**
 * 
 */
class UTextBlock;

UCLASS()
class LAKAYA_API UIndividualLiveScoreBoardWidget : public UIndividualBaseWidget
{
	GENERATED_BODY()
	
public:
	void SetScoreBoardPlayerName(const TArray<FPlayerData>& PlayerDataArray);
	void SetScoreBoardPlayerAIName(const TArray<FPlayerAIData>& PlayerDataArray);

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	// UTextBlock* Score1stNameTextBlock;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	// UTextBlock* Score2ndNameTextBlock;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	// UTextBlock* Score3rdNameTextBlock;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	// UTextBlock* Score4thNameTextBlock;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	// UTextBlock* Score5thNameTextBlock;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	// UTextBlock* Score6thNameTextBlock;
};