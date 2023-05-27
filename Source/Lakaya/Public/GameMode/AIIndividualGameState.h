// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/LakayaBaseGameState.h"
#include "UI/IndividualWidget/AIIndividualScoreBoardWidget.h"
#include "AIIndividualGameState.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AAIIndividualGameState : public ALakayaBaseGameState
{
	GENERATED_BODY()

public:
	AAIIndividualGameState();

	void SetScoreBoardPlayerName(const TArray<FPlayerAIData>& PlayerDataArray);
	void SetAIIndividualWinner();
	
	APlayerState* PlayerState;
	
	TArray<FPlayerAIData> FPlayerAIDataArray;

	FPlayerAIData PlayerAIData;
	FPlayerAIData PlayerAI1Data;
	FPlayerAIData PlayerAI2Data;
	FPlayerAIData PlayerAI3Data;
	FPlayerAIData PlayerAI4Data;
	FPlayerAIData PlayerAI5Data;
	FPlayerAIData PlayerAI6Data;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAIIndividualScoreBoardWidget> AIIndividualScoreBoardWidgettClass;

	TWeakObjectPtr<UAIIndividualScoreBoardWidget> AIIndividualScoreBoardWidget;

	
};
