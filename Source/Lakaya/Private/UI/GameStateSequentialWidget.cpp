// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameStateSequentialWidget.h"

#include "CommonActivatableWidget.h"
#include "CommonActivatableWidgetSwitcher.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
#include "UI/IntroWidget.h"
#include "UI/LoadingWidget.h"

void UGameStateSequentialWidget::SwitchTo(UCommonActivatableWidget* Widget, bool bForce)
{
	if (ensure(IsValid(WidgetSwitcher)) && IsValid(Widget))
	{
		if (!WidgetSwitcher->HasChild(Widget))
		{
			if (!bForce)
			{
				return;
			}
			WidgetSwitcher->AddChild(Widget);
		}

		WidgetSwitcher->SetActiveWidget(Widget);
	}
}

void UGameStateSequentialWidget::SwitchToLoading()
{
	SwitchTo(GetLoadingWidget());
}

void UGameStateSequentialWidget::SwitchToCharacterSelect()
{
	SwitchTo(GetCharacterSelectWidget());
}

void UGameStateSequentialWidget::SwitchToGameIntro()
{
	SwitchTo(GetGameIntroWidget());
}

void UGameStateSequentialWidget::SwitchToInGameOverlay()
{
	SwitchTo(GetInGameOverlayWidget());
}

void UGameStateSequentialWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	WidgetSwitcher->OnActiveWidgetIndexChanged.AddUObject(this, &ThisClass::OnActiveWidgetIndexChanged);
}

void UGameStateSequentialWidget::OnActiveWidgetIndexChanged(UWidget* Widget, int32 Index)
{
	OnActiveWidgetChanged(CastChecked<UCommonActivatableWidget>(Widget));
}
