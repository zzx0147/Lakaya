// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/AIIndividualScoreBoard.h"

#include "Character/LakayaBasePlayerState.h"
#include "Components/VerticalBox.h"
#include "UI/AIIndividualScoreBoardElement.h"
#include "UI/ScoreBoardElement.h"

UAIIndividualScoreBoard::UAIIndividualScoreBoard(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ElementClass = UScoreBoardElement::StaticClass();
}

void UAIIndividualScoreBoard::NativeConstruct()
{
	Super::NativeConstruct();

	AIIndividualTeamBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("AIIndividualTeamVerticalBox")));
	
	if (!AIIndividualTeamBox.IsValid()) UE_LOG(LogInit, Error, TEXT("UAIIndividualScoreBoard::AIIndividualTeamBox was nullptr!"));
	if (!ElementClass) UE_LOG(LogInit, Error, TEXT("UAIIndividualScoreBoard::ElementClass was nullptr!"));
}

void UAIIndividualScoreBoard::RegisterPlayer(APlayerState* PlayerState)
{
	Super::RegisterPlayer(PlayerState);

	UE_LOG(LogTemp, Warning, TEXT("RegisterPlayer Called."));
	
	const auto LakayaState = Cast<ALakayaBasePlayerState>(PlayerState);
	const auto Element = CreateWidget<UAIIndividualScoreBoardElement>(this, ElementClass);
	if (!LakayaState || !Element) return;
	UE_LOG(LogScript, Log, TEXT("UAIIndividualScoreBoard::RegisterPlayer Called"));

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
	// TODO : AI 개인전에서 애들이 None 으로 팀이 들어가있어서 이렇게 처리함 나중에 개인전 팀 할당 체크좀 해줘용

	LakayaState->OnKillCountChanged.AddUObject(this, &UAIIndividualScoreBoard::OnKillCountChanged);

	AIIndividualTeamBox->AddChildToVerticalBox(Element);
	Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Element->Individual_BackGround_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
	LakayaState->OnPlayerNameChanged.AddUObject(Element, &UScoreBoardElement::SetPlayerName);
	LakayaState->OnKillCountChanged.AddUObject(Element, &UScoreBoardElement::SetKillCount);
	LakayaState->OnDeathCountChanged.AddUObject(Element, &UScoreBoardElement::SetDeathCount);
	
	Element->SetPlayerName(LakayaState->GetPlayerName());
	Element->SetDeathCount(LakayaState->GetDeathCount());
	Element->SetKillCount(LakayaState->GetKillCount());
	Element->IndividualKillCount = LakayaState->GetKillCount();
	OnKillCountChanged(LakayaState->GetKillCount());
	
	LakayaState->OnKillCountChanged.AddUObject(this, &UAIIndividualScoreBoard::OnKillCountChanged);
}

void UAIIndividualScoreBoard::OnKillCountChanged(const uint16& NewKillCount)
{
	TArray<UAIIndividualScoreBoardElement*> Elements;

	// AIIndividualTeamBox에서 생성된 모든 Element들을 가져옴
	for (int32 i = 0; i < AIIndividualTeamBox->GetChildrenCount(); i++)
	{
		UAIIndividualScoreBoardElement* Element = Cast<UAIIndividualScoreBoardElement>(AIIndividualTeamBox->GetChildAt(i));
		if (Element)
		{
			Elements.Add(Element);
		}
	}

	// Element들을 킬 카운트를 기준으로 정렬 (내림차순)
	Elements.Sort([](const UAIIndividualScoreBoardElement& A, const UAIIndividualScoreBoardElement& B) {
		return A.IndividualKillCount > B.IndividualKillCount;
	});
	
	// 정렬된 Element들을 AIIndividualTeamBox에 추가 (상단에 위치)
	for (int32 i = 0; i < Elements.Num(); i++)
	{
		UAIIndividualScoreBoardElement* Element = Elements[i];
		if (Element)
		{
			Element->SetIndividualRank(i + 1); // 랭크 설정
			AIIndividualTeamBox->RemoveChild(Element);
			AIIndividualTeamBox->AddChild(Element);
		}
	}
}
