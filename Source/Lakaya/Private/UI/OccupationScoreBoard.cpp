// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OccupationScoreBoard.h"

#include "Character/LakayaBasePlayerState.h"
#include "Components/VerticalBox.h"
#include "UI/ScoreBoardElement.h"

UOccupationScoreBoard::UOccupationScoreBoard(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ElementClass = UScoreBoardElement::StaticClass();
}

void UOccupationScoreBoard::RegisterPlayer(APlayerState* PlayerState)
{
	Super::RegisterPlayer(PlayerState);

	const auto LakayaState = Cast<ALakayaBasePlayerState>(PlayerState);
	const auto Element = CreateWidget<UScoreBoardElement>(this, ElementClass);
	if (!LakayaState || !Element) return;
	UE_LOG(LogScript, Log, TEXT("UOccupationScoreBoard::RegisterPlayer Called"));

	switch (LakayaState->GetTeam())
	{
	case EPlayerTeam::A:
		if (ATeamBox.IsValid())
		{
			ATeamBox->AddChildToVerticalBox(Element);
			Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		break;
	case EPlayerTeam::B:
		if (BTeamBox.IsValid())
		{
			BTeamBox->AddChildToVerticalBox(Element);
			Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		break;
	case EPlayerTeam::None:
		Element->AddToViewport();
		Element->SetVisibility(ESlateVisibility::Collapsed);
		break;
	default:
		UE_LOG(LogScript, Error, TEXT("Invalid team value on UOccupationScoreBoard::RegisterPlayer()!"));
		Element->RemoveFromRoot();
		return;
	}

	// 엘리먼트 데이터 바인딩
	LakayaState->OnTeamChanged.AddLambda([this, Element](const EPlayerTeam& ArgTeam)
	{
		// A팀으로 변경되면 B팀의 버티컬 박스에서 엘리먼트를 제거하고 A팀 버티컬 박스에 엘리먼트를 추가합니다.
		if (ArgTeam == EPlayerTeam::A)
		{
			if (ATeamBox.IsValid() && !ATeamBox->HasChild(Element)) ATeamBox->AddChildToVerticalBox(Element);
			if (BTeamBox.IsValid()) BTeamBox->RemoveChild(Element);
			Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		// B팀으로 변경되면 A팀의 버티컬 박스에서 엘리먼트를 제거하고 B팀 버티컬 박스에 엘리먼트를 추가합니다.
		else if (ArgTeam == EPlayerTeam::B)
		{
			if (ATeamBox.IsValid()) ATeamBox->RemoveChild(Element);
			if (BTeamBox.IsValid() && !BTeamBox->HasChild(Element)) BTeamBox->AddChildToVerticalBox(Element);
			Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else UE_LOG(LogScript, Error, TEXT("Invalid team value on OnTeamChanged lambda handler!"));
	});
	LakayaState->OnDeathCountChanged.AddUObject(Element, &UScoreBoardElement::SetDeathCount);
	LakayaState->OnKillCountChanged.AddUObject(Element, &UScoreBoardElement::SetKillCount);

	//TODO: 플레이어 이름이 잘 표시되는지 검증 필요. 리플리케이트가 늦어서 플레이어 이름이 제대로 표시되지 않는다면 플레이어 이름 변경 이벤트 선언 필요
	Element->SetPlayerName(FText::FromString(LakayaState->GetPlayerName()));
	Element->SetDeathCount(LakayaState->GetDeathCount());
	Element->SetKillCount(LakayaState->GetKillCount());
}

void UOccupationScoreBoard::NativeConstruct()
{
	Super::NativeConstruct();

	ATeamBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("ATeamVerticalBox")));
	BTeamBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("BTeamVerticalBox")));

	if (!ATeamBox.IsValid()) UE_LOG(LogInit, Error, TEXT("UOccupationScoreBoard::ATeamBox was nullptr!"));
	if (!BTeamBox.IsValid()) UE_LOG(LogInit, Error, TEXT("UOccupationScoreBoard::BTeamBox was nullptr!"));
	if (!ElementClass) UE_LOG(LogInit, Error, TEXT("UOccupationScoreBoard::ElementClass was nullptr!"));
}
