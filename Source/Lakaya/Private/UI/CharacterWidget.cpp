// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterWidget.h"

#include "UI/DynamicCrossHairWidget.h"

void UCharacterWidget::SetAliveState(const bool& IsAlive)
{
	CrosshairWidget->SetVisibility(IsAlive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
}

