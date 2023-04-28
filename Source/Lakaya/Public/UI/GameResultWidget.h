// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MatchStateWidget.h"
#include "Components/TextBlock.h"
#include "Occupation/PlayerTeam.h"
#include "GameResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UGameResultWidget : public UMatchStateWidget
{
	GENERATED_BODY()

public:
	virtual bool OnMatchStart() override { return true; }
	virtual bool OnMatchEnding() override;

protected:
	virtual void NativeConstruct() override;

private:
	void OnChangeWinner(const EPlayerTeam& NewWinner);

	UPROPERTY(meta =(BindWidget))
	UTextBlock* GameResultWidgetText;

	class AOccupationGameState* OccupationGameState;
};
