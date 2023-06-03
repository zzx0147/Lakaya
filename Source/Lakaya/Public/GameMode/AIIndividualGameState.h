// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/LakayaBaseGameState.h"
#include "UI/IndividualWidget/IndividualLiveScoreBoardWidget.h"
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

	void SetScoreBoardPlayerAIName(const TArray<FPlayerAIData>& PlayerAIDataArray);
	void SetAIIndividualWinner();
	
	APlayerState* PlayerState;
	
	TArray<FPlayerAIData> FPlayerAIDataArray;

	FPlayerAIData PlayerAIData;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	TWeakObjectPtr<class UIndividualLiveScoreBoardWidget> AIIndividualLiveScoreBoardWidget;
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UIndividualLiveScoreBoardWidget> AIIndividualLiveScoreBoardWidgetClass;
	
};
