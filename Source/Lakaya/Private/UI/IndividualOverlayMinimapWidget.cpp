// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/IndividualOverlayMinimapWidget.h"

void UIndividualOverlayMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// PlayerState = Cast<ALakayaBasePlayerState>(GetOwningPlayerState());
}

void UIndividualOverlayMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 자기 자신의 위치를 업데이트 해줍니다.
	// UpdatePlayerPosition(CurrentTeam, PlayerState);
}