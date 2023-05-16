// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameTimeWidget.h"

#include "GameFramework/GameStateBase.h"

UGameTimeWidget::UGameTimeWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TimeTextFormat = FText::FromString(TEXT("{0}:{1}"));
}

void UGameTimeWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameTimeWidgetText = Cast<UTextBlock>(GetWidgetFromName(TEXT("GameTimeWidgetText")));
	if (GameTimeWidgetText == nullptr) UE_LOG(LogTemp, Warning, TEXT("GameTimeWidgetText is null."));
}

void UGameTimeWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const auto RawRemainTime = DestinationTime - GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
	const auto RemainSeconds = RawRemainTime > 0.f ? FMath::RoundToInt(RawRemainTime) : 0;
	GameTimeWidgetText->SetText(FText::Format(TimeTextFormat, RemainSeconds / 60, RemainSeconds % 60));
}

void UGameTimeWidget::SetWidgetTimer(const float& ArgDestinationTime)
{
	DestinationTime = ArgDestinationTime;
}