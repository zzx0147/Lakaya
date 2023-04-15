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

	// 매치에 적절한 위젯을 생성합니다.
	virtual void SetupMatchWidget();

	// 매치를 위해 생성된 위젯들을 제거합니다.
	virtual void RemoveMatchWidget();

private:
	UPROPERTY(EditAnywhere)
	UDataTable* MatchWidgetDataTable;

	TArray<UUserWidget*> MatchWidgets;
};
