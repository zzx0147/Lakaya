// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AIIndividualLiveScoreBoardWidget.h"

#include "AI/AiDroneController.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "UI/ScoreBoardElement.h"
#include "UI/IndividualWidget/IndividualLiveScoreElement.h"

UAIIndividualLiveScoreBoardWidget::UAIIndividualLiveScoreBoardWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ElementClass = UAIIndividualLiveScoreBoardWidget::StaticClass();
}

void UAIIndividualLiveScoreBoardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AIIndividualLiveBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("AIIndividualLiveVerticalBox")));

	if (!AIIndividualLiveBox.IsValid()) UE_LOG(LogInit, Error, TEXT("UIndividualLiveScoreBoardWidget::AIIndividualLiveBox was nullptr!"));
	if (!ElementClass) UE_LOG(LogInit, Error, TEXT("UIndividualLiveScoreBoardWidget::ElementClass was nullptr!"));
}

void UAIIndividualLiveScoreBoardWidget::RegisterPlayer(APlayerState* PlayerState)
{
	Super::RegisterPlayer(PlayerState);

	UE_LOG(LogTemp, Warning, TEXT("RegisterPlayer Called."));

	const auto LakayaState = Cast<ALakayaBasePlayerState>(PlayerState);

	if(LakayaState->GetOwningController() != Cast<AAiDroneController>(LakayaState->GetOwningController()))
	{
		const auto Element = CreateWidget<UIndividualLiveScoreElement>(this, ElementClass);
		if (!LakayaState || !Element) return;
		UE_LOG(LogScript, Log, TEXT("UAIIndividualLiveScoreBoardWidget::RegisterPlayer Called"));

		switch (LakayaState->GetTeam())
		{
		case ETeam::None:
			Element->AddToViewport();
			Element->SetVisibility(ESlateVisibility::Collapsed);
			break;
		case ETeam::Individual:
			Element->AddToViewport();
			Element->SetVisibility(ESlateVisibility::Collapsed);
			break;
		default:
			UE_LOG(LogScript, Error, TEXT("Invalid team value on UAIIndividualScoreBoard::RegisterPlayer()!"));
			Element->RemoveFromRoot();
			return;
		}
		// 엘리먼트 데이터 바인딩

		LakayaState->OnKillCountChanged.AddUObject(this, &UAIIndividualLiveScoreBoardWidget::OnKillCountChanged);

		AIIndividualLiveBox->AddChildToVerticalBox(Element);
		Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Element->RankBox_Img->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		
		if (LakayaState->GetPlayerController()) Element->SetPlayerCheckVisible();
		else Element->SetPlayerCheckHidden();

		LakayaState->OnPlayerNameChanged.AddUObject(Element, &UScoreBoardElement::SetPlayerName);
		LakayaState->OnKillCountChanged.AddUObject(Element, &UScoreBoardElement::SetKillCount);
		
		Element->SetPlayerName(LakayaState->GetPlayerName());
		Element->SetKillCount(LakayaState->GetKillCount());
		Element->IndividualKillCount = LakayaState->GetKillCount();
		OnKillCountChanged(LakayaState->GetKillCount());
	
		LakayaState->OnKillCountChanged.AddUObject(this, &UAIIndividualLiveScoreBoardWidget::OnKillCountChanged);
	}
}

void UAIIndividualLiveScoreBoardWidget::OnKillCountChanged(const uint16& NewKillCount)
{
	TArray<UIndividualLiveScoreElement*> Elements;

	// AIIndividualTeamBox에서 생성된 모든 Element들을 가져옴
	for (int32 i = 0; i < AIIndividualLiveBox->GetChildrenCount(); i++)
	{
		UIndividualLiveScoreElement* Element = Cast<UIndividualLiveScoreElement>(AIIndividualLiveBox->GetChildAt(i));
		if (Element)
		{
			Elements.Add(Element);
		}
	}

	// Element들을 킬 카운트를 기준으로 정렬 (내림차순)
	Elements.Sort([](const UIndividualLiveScoreElement& A, const UIndividualLiveScoreElement& B) {
		return A.IndividualKillCount > B.IndividualKillCount;
	});
	
	// 정렬된 Element들을 AIIndividualTeamBox에 추가 (상단에 위치)
	for (int32 i = 0; i < Elements.Num(); i++)
	{
		UIndividualLiveScoreElement* Element = Elements[i];
		if (Element)
		{
			if(i == 0) Element->Set1stElementBoxImage();
			else Element->SetOtherElementBoxImage();
			
			Element->SetIndividualRank(i + 1); // 랭크 설정
			AIIndividualLiveBox->RemoveChild(Element);
			AIIndividualLiveBox->AddChild(Element);
		}
	}
}


