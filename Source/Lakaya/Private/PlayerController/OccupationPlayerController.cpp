// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/OccupationPlayerController.h"

#include "Engine/DataTable.h"
#include "PlayerController/MatchWidgetData.h"


AOccupationPlayerController::AOccupationPlayerController()
{
	//TODO: 데이터 테이블 가져오기
}

void AOccupationPlayerController::SetupMatchWidget()
{
	const auto Data = MatchWidgetDataTable->FindRow<FMatchWidgetData>(TEXT("Occupation"),TEXT("OccupationWidgetSetup"));
	if (!Data) return;

	MatchWidgets.Reserve(Data->WidgetList.Num());
	for (auto& WidgetClass : Data->WidgetList)
		MatchWidgets.Emplace(CreateViewportWidget<UUserWidget>(WidgetClass));
}

void AOccupationPlayerController::RemoveMatchWidget()
{
	for (const auto& Widget : MatchWidgets) Widget->RemoveFromParent();
	MatchWidgets.Empty();
}
