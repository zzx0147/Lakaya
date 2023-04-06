// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameMode/OccupationGameState.h"
#include "TeamScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UTeamScoreWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ATeamScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BTeamScoreText;
	
public:
	UFUNCTION()
	void SetTeamScoreWidget(EOccupationGameState ChangeGameState);

public:
	UFUNCTION()
	void ReMoveTeamScoreWidget(EOccupationGameState ChangeGameState);
	
	UFUNCTION()
	void OnChangeATeamScore(float NewScore);
	
	UFUNCTION()
	void OnChangeBTeamScore(float NewScore);
};