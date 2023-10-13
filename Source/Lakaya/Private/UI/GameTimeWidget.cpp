// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/GameTimeWidget.h"

#include "GameFramework/GameStateBase.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "Kismet/KismetTextLibrary.h"

UGameTimeWidget::UGameTimeWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TimeTextFormat = FText::FromString(TEXT("{0}:{1}"));

	MatchWaitToStart = true;
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
	GameTimeWidgetText->SetText(FText::Format(TimeTextFormat,
		UKismetTextLibrary::Conv_Int64ToText(RemainSeconds / 60,false,false,2),
		UKismetTextLibrary::Conv_Int64ToText(RemainSeconds % 60,false,false,2)));
}

void UGameTimeWidget::SetWidgetTimer(const float& ArgDestinationTime)
{
	DestinationTime = ArgDestinationTime;
}
