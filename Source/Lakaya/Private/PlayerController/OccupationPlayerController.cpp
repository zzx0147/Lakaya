// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/OccupationPlayerController.h"

#include "Engine/DataTable.h"
#include "PlayerController/MatchWidgetData.h"
#include "UI/MatchStateWidget.h"


AOccupationPlayerController::AOccupationPlayerController()
{
	static const ConstructorHelpers::FObjectFinder<UDataTable> WidgetTableFinder(
		TEXT("/Script/Engine.DataTable'/Game/Dev/Yongwoo/DataTables/DT_MatchWidgetData.DT_MatchWidgetData'"));

	if (WidgetTableFinder.Succeeded()) MatchWidgetDataTable = WidgetTableFinder.Object;
}

void AOccupationPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (!IsLocalController()) return;
	const auto Data = MatchWidgetDataTable->FindRow<FMatchWidgetData>(TEXT("Occupation"),TEXT("OccupationWidgetSetup"));
	if (!Data) return;

	MatchWidgets.Reserve(Data->WidgetList.Num());
	for (auto& WidgetClass : Data->WidgetList)
	{
		auto* MatchStateWidget = CreateViewportWidget<UMatchStateWidget>(WidgetClass, ESlateVisibility::Hidden);
		MatchStateWidget->OnBeginPlay();
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
