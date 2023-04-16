// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/OccupationPlayerController.h"

#include "Engine/DataTable.h"
#include "PlayerController/MatchWidgetData.h"
#include "UI/MatchStateWidget.h"


AOccupationPlayerController::AOccupationPlayerController()
{
	//TODO: 데이터 테이블 가져오기
}

void AOccupationPlayerController::BeginPlay()
{
	Super::BeginPlay();
	const auto Data = MatchWidgetDataTable->FindRow<FMatchWidgetData>(TEXT("Occupation"),TEXT("OccupationWidgetSetup"));
	if (!Data) return;

	MatchWidgets.Reserve(Data->WidgetList.Num());
	for (auto& WidgetClass : Data->WidgetList)
	{
		auto* MatchStateWidget = CreateViewportWidget<UMatchStateWidget>(WidgetClass, ESlateVisibility::Hidden);
		MatchWidgets.Emplace(MatchStateWidget);
	}
}

void AOccupationPlayerController::OnMatchStart()
{
	RemoveWidgets.Reserve(MatchWidgets.Num());

	for (auto& Widget : MatchWidgets)
		if (!Widget->OnMatchStart()) RemoveWidgets.Emplace(Widget);

	for (auto& Widget : RemoveWidgets)
	{
		Widget->RemoveFromParent();
		MatchWidgets.RemoveSwap(Widget, false);
	}
	MatchWidgets.Shrink();
	RemoveWidgets.Empty(MatchWidgets.Num());
}

void AOccupationPlayerController::OnMatchEnding()
{
	RemoveWidgets.Reserve(MatchWidgets.Num());

	for (auto& Widget : MatchWidgets)
		if (!Widget->OnMatchEnding()) RemoveWidgets.Emplace(Widget);

	for (auto& Widget : RemoveWidgets)
	{
		Widget->RemoveFromParent();
		MatchWidgets.RemoveSwap(Widget, false);
	}
	MatchWidgets.Shrink();
	RemoveWidgets.Empty();
}
