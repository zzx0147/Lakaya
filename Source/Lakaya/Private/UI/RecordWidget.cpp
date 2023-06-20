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

	//Insert Test code
	// FPlayerStats a1,a2,a3,p1,p2,p3;
	// a1.Death = 1; a1.Kill = 6; a1.OccupationCount = 2; a1.OccupationTickCount = 7; a1.PlayerID = TEXT("00000000000000000"); a1.PlayerName = TEXT("Test1");
	// a2.Death = 2; a2.Kill = 5; a2.OccupationCount = 3; a2.OccupationTickCount = 6; a2.PlayerID = TEXT("00000000000000001"); a2.PlayerName = TEXT("Test2");
	// a3.Death = 3; a3.Kill = 4; a3.OccupationCount = 4; a3.OccupationTickCount = 5; a3.PlayerID = TEXT("00000000000000002"); a3.PlayerName = TEXT("Test3");
	//
	// p1.Death = 4; p1.Kill = 3; p1.OccupationCount = 5; p1.OccupationTickCount = 4; p1.PlayerID = TEXT("00000000000000003"); p1.PlayerName = TEXT("Test4");
	// p2.Death = 5; p2.Kill = 2; p2.OccupationCount = 6; p2.OccupationTickCount = 3; p2.PlayerID = TEXT("00000000000000004"); p2.PlayerName = TEXT("Test5");
	// p3.Death = 6; p3.Kill = 1; p3.OccupationCount = 7; p3.OccupationTickCount = 2; p3.PlayerID = TEXT("00000000000000005"); p3.PlayerName = TEXT("Test6");
	//
	// FRecordResultStruct temp;
	// temp.StartTime = 20.0f;
	// temp.Duration = 20.0f;
	// temp.WinTeam = EPlayerTeam::A;
	// temp.AntiPlayers.Emplace(a1);
	// temp.AntiPlayers.Emplace(a2);
	// temp.AntiPlayers.Emplace(a3);
	// temp.ProPlayers.Emplace(p1);
	// temp.ProPlayers.Emplace(p2);
	// temp.ProPlayers.Emplace(p3);
	//
	// if(!EOSGameInstance->IsSocketConnected()) EOSGameInstance->SendRecordResultData(temp);
	RequestShowRecord();
}

void URecordWidget::RequestShowRecord()
{
	if(EOSGameInstance != nullptr)
	{
		if(!EOSGameInstance->IsSocketConnected()) return;

		bRequestShowRecord = EOSGameInstance->RequestShowRecord();
	}
}

void URecordWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if(bRequestShowRecord && EOSGameInstance->IsSocketConnected())
	{
		if(EOSGameInstance->HasPendingData())
		{
			const auto Results = EOSGameInstance->RecvMatchResultRecord();

			
			
			
			//받은 결과 보여주기
			
		}
	}
}
