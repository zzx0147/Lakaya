// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GameScoreBoardWidget.h"
#include "AIIndividualScoreBoard.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UAIIndividualScoreBoard : public UGameScoreBoardWidget
{
	GENERATED_BODY()

public:
	explicit UAIIndividualScoreBoard(const FObjectInitializer& ObjectInitializer);
	
	virtual void RegisterPlayer(APlayerState* PlayerState) override;
	void OnKillCountChanged(const uint16& NewKillCount);

protected:
	virtual void NativeConstruct() override;

	// 스코어보드에서 각각의 캐릭터 정보를 표시하는 엘리먼트의 클래스를 지정합니다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UScoreBoardElement> ElementClass;

	TWeakObjectPtr<class UVerticalBox> AIIndividualTeamBox;
};
