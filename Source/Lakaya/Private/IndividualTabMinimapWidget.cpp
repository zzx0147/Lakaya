// Fill out your copyright notice in the Description page of Project Settings.


#include "IndividualTabMinimapWidget.h"

// TODO: 헤더파일에 선언된 기능을 구현해야 합니다.
void UIndividualTabMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerState = Cast<ALakayaBasePlayerState>(GetOwningPlayerState());
}

void UIndividualTabMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 자기 자신의 위치를 업데이트 해줍니다.
	UpdatePlayerPosition(CurrentTeam, PlayerState);
}