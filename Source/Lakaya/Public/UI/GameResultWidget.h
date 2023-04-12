// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameMode/OccupationGameState.h"
#include "GameResultWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UGameResultWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta =(BindWidget))
	UTextBlock* GameResultWidgetText;

public:
	UFUNCTION()
	void ReMoveLoadingWidget(EOccupationGameState ChangeGameState);

public :
	void OnChangeWinner(EOccupationWinner NewWinner);

private:
	AOccupationGameState* OccupationGameState;
};