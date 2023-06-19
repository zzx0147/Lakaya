// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RecordWidget.h"

#include "EOS/EOSGameInstance.h"


URecordWidget::URecordWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bRequestShowRecord = false;
}

void URecordWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EOSGameInstance = Cast<UEOSGameInstance>(GetWorld()->GetGameInstance());
	
}

void URecordWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	if(InVisibility != ESlateVisibility::Visible) return;
	if(!EOSGameInstance->IsSocketConnected()) EOSGameInstance->Connect();
}

void URecordWidget::RequestShowRecord()
{
	if(EOSGameInstance != nullptr)
	{
		if(!EOSGameInstance->IsSocketConnected()) return;

		EOSGameInstance->RequestShowRecord();
		bRequestShowRecord = true;
	}
}

void URecordWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(bRequestShowRecord && EOSGameInstance->IsSocketConnected())
	{
		if(EOSGameInstance->HasPendingData())
		{
			const auto Results = EOSGameInstance->RecvRecordResultData();

			//받은 결과 보여주기
			
		}
	}
}
