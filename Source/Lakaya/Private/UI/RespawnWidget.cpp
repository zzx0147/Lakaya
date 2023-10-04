// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RespawnWidget.h"

#include "Components/TextBlock.h"
#include "UI/RadialProgressBar.h"

URespawnWidget::URespawnWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bUseTick = false;
}

void URespawnWidget::StartRespawnProgress(const float& ReservedRespawnTime, const float& CurrentTime)
{
	RemainRespawnTime = MaxRespawnTime = ReservedRespawnTime - CurrentTime;
	if(RemainRespawnTime <= 0.0f) return;
	
	SetVisibility(ESlateVisibility::HitTestInvisible);
	bUseTick = true;
}

void URespawnWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(!bUseTick) return;
	
	RemainRespawnTime -= InDeltaTime;

	const float RemainPercent = RemainRespawnTime / MaxRespawnTime;
	RadialProgressBar->SetPercent(1.0f - RemainPercent);

	RemainTimeText->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), RemainRespawnTime)));
	if (RemainPercent <= 0.0f)
	{
		SetVisibility(ESlateVisibility::Hidden);
		bUseTick = false;
	}
}
