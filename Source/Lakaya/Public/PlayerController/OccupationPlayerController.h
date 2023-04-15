// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattlePlayerController.h"
#include "OccupationPlayerController.generated.h"

UCLASS()
class LAKAYA_API AOccupationPlayerController : public ABattlePlayerController
{
	GENERATED_BODY()

public:
	AOccupationPlayerController();

protected:
	virtual void BeginPlay() override;

public:
	// 매치가 시작되고 호출됩니다.
	virtual void OnMatchStart();

	// 매치가 종료되고 호출됩니다.
	virtual void OnMatchEnding();

private:
	UPROPERTY(EditAnywhere)
	UDataTable* MatchWidgetDataTable;

	TArray<class UMatchStateWidget*> MatchWidgets;
	TArray<UMatchStateWidget*> RemoveWidgets;
};
