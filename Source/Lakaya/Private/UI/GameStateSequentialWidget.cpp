// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameStateSequentialWidget.h"

#include "CommonActivatableWidget.h"
#include "CommonActivatableWidgetSwitcher.h"

void UGameStateSequentialWidget::SwitchTo(UCommonActivatableWidget* Widget)
{
	if (ensure(IsValid(WidgetSwitcher)) && IsValid(Widget))
	{
		if (!WidgetSwitcher->HasChild(Widget))
		{
			WidgetSwitcher->AddChild(Widget);
		}

		WidgetSwitcher->SetActiveWidget(Widget);
	}
}
