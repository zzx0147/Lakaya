// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/LakayaBaseGameState.h"
#include "UI/GameAIIndividualScoreBoardWidget.h"
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

	void SetScoreBoardPlayerName(const TArray<FPlayerData>& PlayerDataArray);
	
	APlayerState* PlayerState;
	
	TArray<FPlayerData> FPlayerDataArray;

	FPlayerData PlayerData;
	FPlayerData Player1Data;
	FPlayerData Player2Data;
	FPlayerData Player3Data;
	FPlayerData Player4Data;
	FPlayerData Player5Data;
	FPlayerData Player6Data;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameAIIndividualScoreBoardWidget> AIIndividualScoreBoardWidgettClass;

	TWeakObjectPtr<UGameAIIndividualScoreBoardWidget> AIIndividualScoreBoardWidget;

	
};
