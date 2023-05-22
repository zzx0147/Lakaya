// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameScoreBoardWidget.h"
#include "OccupationScoreBoard.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UOccupationScoreBoard : public UGameScoreBoardWidget
{
	GENERATED_BODY()

public:
	explicit UOccupationScoreBoard(const FObjectInitializer& ObjectInitializer);
	
	virtual void RegisterPlayer(APlayerState* PlayerState) override;
protected:
	virtual void NativeConstruct() override;

	// 스코어보드에서 각각의 플레이어 정보를 표시하는 엘리먼트의 클래스를 지정합니다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UScoreBoardElement> ElementClass;

	TWeakObjectPtr<class UVerticalBox> ATeamBox;
	TWeakObjectPtr<UVerticalBox> BTeamBox;
};
