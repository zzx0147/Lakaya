// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GameScoreBoardWidget.h"
#include "AIIndividualLiveScoreBoardWidget.generated.h"

/**
 * 
 */
class UTextBlock;
class UImage;
class UVerticalBox;
class UIndividualLiveScoreElement;

UCLASS()
class LAKAYA_API UAIIndividualLiveScoreBoardWidget : public UGameScoreBoardWidget
{
	GENERATED_BODY()
	
public:
	explicit UAIIndividualLiveScoreBoardWidget(const FObjectInitializer& ObjectInitializer);
	
	virtual void RegisterPlayer(APlayerState* PlayerState) override;
	void OnKillCountChanged(const uint16& NewKillCount);
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UIndividualLiveScoreElement> ElementClass;

	TWeakObjectPtr<UVerticalBox> AIIndividualLiveBox;

};
