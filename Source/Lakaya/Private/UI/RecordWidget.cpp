// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RecordWidget.h"

#include "Components/ScrollBox.h"
#include "EOS/EOSGameInstance.h"
#include "UI/RecordRow.h"


URecordWidget::URecordWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShowRecordRequested = false;
	RecordRowNumber = 10;
}

void URecordWidget::NativeConstruct()
{
	Super::NativeConstruct();
	EOSGameInstance = Cast<UEOSGameInstance>(GetWorld()->GetGameInstance());

	RecordScrollViewWidget = Cast<UScrollBox>(GetWidgetFromName(TEXT("RecordScrollView")));

	if (!RecordScrollViewWidget) return;
	if (!RecordRowClass) return;

	for (int i = 0; i < 10; ++i)
	{
		const auto Temp = CreateWidget<URecordRow>(this, RecordRowClass);
		Temp->SetParentWidget(this);
		RecordScrollViewWidget->AddChild(Temp);
		RecordRowWidgetArray.Emplace(Temp);
	}
}

void URecordWidget::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	if (InVisibility != ESlateVisibility::Visible) return;
	if (!EOSGameInstance->IsSocketConnected()) EOSGameInstance->Connect();

	//Insert Test code
	FPlayerStats a1,a2,a3,p1,p2,p3;
	a1.Death = 1; a1.Kill = 6; a1.OccupationCount = 2; a1.OccupationTickCount = 7; a1.PlayerID = TEXT("76561198114451350"); a1.PlayerName = TEXT("Test1");
	a2.Death = 2; a2.Kill = 5; a2.OccupationCount = 3; a2.OccupationTickCount = 6; a2.PlayerID = TEXT("00000000000000001"); a2.PlayerName = TEXT("Test2");
	a3.Death = 3; a3.Kill = 4; a3.OccupationCount = 4; a3.OccupationTickCount = 5; a3.PlayerID = TEXT("00000000000000002"); a3.PlayerName = TEXT("Test3");
	
	p1.Death = 4; p1.Kill = 3; p1.OccupationCount = 5; p1.OccupationTickCount = 4; p1.PlayerID = TEXT("00000000000000003"); p1.PlayerName = TEXT("Test4");
	p2.Death = 5; p2.Kill = 2; p2.OccupationCount = 6; p2.OccupationTickCount = 3; p2.PlayerID = TEXT("00000000000000004"); p2.PlayerName = TEXT("Test5");
	p3.Death = 6; p3.Kill = 1; p3.OccupationCount = 7; p3.OccupationTickCount = 2; p3.PlayerID = TEXT("00000000000000005"); p3.PlayerName = TEXT("Test6");
	
	FMatchResultStruct temp;
	temp.StartTime = 1687291289;
	temp.Duration = 180.0f;
	temp.WinTeam = EPlayerTeam::Anti;
	temp.AntiPlayers.Emplace(a1);
	temp.AntiPlayers.Emplace(a2);
	temp.AntiPlayers.Emplace(a3);
	temp.ProPlayers.Emplace(p1);
	temp.ProPlayers.Emplace(p2);
	temp.ProPlayers.Emplace(p3);
	
	// if(EOSGameInstance->IsSocketConnected()) EOSGameInstance->SendMatchResultData(temp);
	
	RequestShowRecord();
}

void URecordWidget::RequestShowRecord()
{
	if (EOSGameInstance != nullptr)
	{
		if (!EOSGameInstance->IsSocketConnected()) return;

		bShowRecordRequested = EOSGameInstance->RequestShowRecord();
		bShowRecordRequested = true;
	}
}

void URecordWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bShowRecordRequested && EOSGameInstance->IsSocketConnected())
	{
		if (EOSGameInstance->HasPendingData())
		{
			auto Results = EOSGameInstance->RecvMatchResultRecord();

			// TArray<FMatchResultStruct> Results; //임시 대체 데이터
			//
			// FMatchResultStruct temp, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9;
			// temp.Duration = 20;
			// temp.StartTime = 1687241283;
			// temp.WinTeam = EPlayerTeam::A;
			// temp.AntiPlayers.Emplace(FPlayerStats(TEXT("76561198114451350"),TEXT("pollux"), 3, 3, 5, 250));
			// temp.AntiPlayers.Emplace(FPlayerStats(TEXT("71111198114451350"),TEXT("pollux2"), 2, 1, 2, 300));
			// temp.AntiPlayers.Emplace(FPlayerStats(TEXT("76522298114451350"),TEXT("pollux3"), 1, 3, 1, 50));
			//
			// temp.ProPlayers.Emplace(FPlayerStats(TEXT("76561123214451350"),TEXT("pollux4"), 3, 3, 2, 20));
			// temp.ProPlayers.Emplace(FPlayerStats(TEXT("71111198114333350"),TEXT("pollux5"), 1, 1, 2, 300));
			// temp.ProPlayers.Emplace(FPlayerStats(TEXT("76522297774451350"),TEXT("pollux6"), 1, 5, 5, 500));
			//
			// temp2 = temp;
			// temp3 = temp;
			// temp4 = temp;
			// temp5 = temp;
			// temp6 = temp;
			// temp7 = temp;
			// temp8 = temp;
			
			// temp9 = temp;
			// temp2.StartTime = 1687241289;
			// temp3.StartTime = 1687241290;
			// temp4.StartTime = 1687241291;
			// temp5.StartTime = 1687241292;
			// temp6.StartTime = 1687241293;
			// temp7.StartTime = 1687241294;
			// temp8.StartTime = 1687241295;
			// temp9.StartTime = 1687241296;
			//
			// Results.Emplace(temp);
			// Results.Emplace(temp2);
			// Results.Emplace(temp3);
			// Results.Emplace(temp4);
			// Results.Emplace(temp5);
			// Results.Emplace(temp6);
			// Results.Emplace(temp7);
			// Results.Emplace(temp8);
			// Results.Emplace(temp9);


			//받은 결과 보여주기
			for (int i = 0; i < RecordRowWidgetArray.Num(); ++i)
			{
				if(!Results.IsValidIndex(i))
				{
					RecordRowWidgetArray[i]->SetVisibility(ESlateVisibility::Collapsed);
					continue;
				}
				
				RecordRowWidgetArray[i]->SetRecordRowData(Results[i].WinTeam,FText::FromString(FDateTime::FromUnixTimestamp
					(Results[i].StartTime).ToString()),FText::AsNumber(Results[i].Duration));
			}
			bShowRecordRequested = false;
		}
	}
}
