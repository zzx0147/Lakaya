// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameMode/IndividualGameState.h"
#include "GameTimeWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UGameTimeWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameTimeWidgetText;

public:
	UFUNCTION()
	void SetGameTimeWidget(EIndividualGameState ChangeGameState);

public:
	UFUNCTION()
	void OnChangeTime(int32 Min, int32 Sec);
};