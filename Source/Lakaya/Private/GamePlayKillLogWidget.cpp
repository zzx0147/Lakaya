// Fill out your copyright notice in the Description page of Project Settings.

#define DO_CHECK 1

#include "GamePlayKillLogWidget.h"
#include "KillLogElement.h"

UGamePlayKillLogWidget::UGamePlayKillLogWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
}

void UGamePlayKillLogWidget::NativeConstruct()
{
	Super::NativeConstruct();


	KillLogElementArray =
	{
		Cast<UKillLogElement>(GetWidgetFromName(TEXT("WBP_KillLogElement1"))),
		Cast<UKillLogElement>(GetWidgetFromName(TEXT("WBP_KillLogElement2"))),
		Cast<UKillLogElement>(GetWidgetFromName(TEXT("WBP_KillLogElement3")))
	};

	for (auto temp : KillLogElementArray) { check(temp != nullptr) }

}

void UGamePlayKillLogWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

}
