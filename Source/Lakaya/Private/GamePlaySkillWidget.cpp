// Fill out your copyright notice in the Description page of Project Settings.

#define DO_CHECK 1

#include "GamePlaySkillWidget.h"
#include "Components/ProgressBar.h"

UGamePlaySkillWidget::UGamePlaySkillWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UGamePlaySkillWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SkillProgressBarArray =
	{
		Cast<UProgressBar>(GetWidgetFromName(TEXT("Skill1_Prb"))),
		Cast<UProgressBar>(GetWidgetFromName(TEXT("Skill2_Prb"))),
	};

	for (auto temp : SkillProgressBarArray) { check(temp != nullptr); }
}

void UGamePlaySkillWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
