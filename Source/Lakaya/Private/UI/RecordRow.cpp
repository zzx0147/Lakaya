// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RecordRow.h"

#include "Components/TextBlock.h"
#include "Occupation/Team.h"

URecordRow::URecordRow(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void URecordRow::NativeConstruct()
{
	Super::NativeConstruct();

	WinTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("WinText")));
	StartTimeTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("StartTimeText")));
	DurationTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("DurationText")));
	UpdateTextBlock();
}

void URecordRow::SetRecordRowData(const ETeam& InWinTeam, const FText& InStartTimeText, const FText& InDurationText)
{
	WinText = FText::FromString(InWinTeam == ETeam::Anti ? TEXT("A") : TEXT("B"));
	StartTimeText = InStartTimeText;
	DurationText = InDurationText;
	UpdateTextBlock();
}

void URecordRow::UpdateTextBlock()
{
	if(WinTextBlock) WinTextBlock->SetText(WinText);
	if(StartTimeTextBlock) StartTimeTextBlock->SetText(StartTimeText);
	if(DurationTextBlock) DurationTextBlock->SetText(DurationText);
}

