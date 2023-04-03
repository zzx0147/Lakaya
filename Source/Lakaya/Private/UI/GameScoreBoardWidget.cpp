// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameScoreBoardWidget.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "UI/ScoreBoardElement.h"
#include "Components/CanvasPanel.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

UGameScoreBoardWidget::UGameScoreBoardWidget(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
}

void UGameScoreBoardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	ScoreBoardPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("ScoreBoard_Pan")));
	if (ScoreBoardPanel == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ScoreBoardPanel is null."));
		return;
	}

	InitScoreBoardElements(6);
}

void UGameScoreBoardWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UGameScoreBoardWidget::InitScoreBoardElements(int8 ElementsNum)
{
	if (ScoreBoardElementArray.Num() != 0) //배열에 이미 엘리먼트가 들어있다면 이미 초기화 된 것이므로 바로 종료
	{
		return;
	}

	//ScoreBoardElement의 블루프린트 위젯 클래스를 로드
	UClass* ScoreBoardElementClass  = LoadClass<UUserWidget>(nullptr, TEXT("/Game/Blueprints/UMG/WBP_ScoreBoardElement.WBP_ScoreBoardElement_C"));

	if (ScoreBoardElementClass  != nullptr)
	{
		for (int8 i = 0; i < ElementsNum; i++)
		{
			UUserWidget* newWidget = CreateWidget<UUserWidget>(this, ScoreBoardElementClass );
			if (newWidget != nullptr)
			{
				ScoreBoardPanel->AddChild(newWidget);
				UCanvasPanelSlot* tempSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(newWidget);
				tempSlot->SetSize(FVector2d(200.0f, 50.0f));
				tempSlot->SetPosition(FVector2d(0.0f, 50.0f * i));
				ScoreBoardElementArray.Add(Cast<UScoreBoardElement>(newWidget));
			}
		}
	}
}