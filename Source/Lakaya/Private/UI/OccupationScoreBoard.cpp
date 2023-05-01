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

	//TODO: 클라이언트에서는 SelectCharacter 매치스테이트에서 아직 팀이 리플리케이트 되지 않았을 수 있으므로 이에 대한 예외처리가 필요합니다.
	switch (const auto& Team = LakayaState->GetTeam())
	{
	case EPlayerTeam::A:
		if (ATeamBox.IsValid()) ATeamBox->AddChildToVerticalBox(Element);
		break;
	case EPlayerTeam::B:
		if (BTeamBox.IsValid()) BTeamBox->AddChildToVerticalBox(Element);
		break;
	default:
		Element->RemoveFromRoot();
		return;
	}

	// 엘리먼트 바인딩
	LakayaState->OnDeathCountChanged.AddUObject(Element, &UScoreBoardElement::SetDeathCount);
	LakayaState->OnKillCountChanged.AddUObject(Element, &UScoreBoardElement::SetKillCount);

	Element->SetPlayerName(FText::FromString(LakayaState->GetPlayerName()));
	Element->SetDeathCount(LakayaState->GetDeathCount());
	Element->SetKillCount(LakayaState->GetKillCount());
	Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UOccupationScoreBoard::NativeConstruct()
{
	Super::NativeConstruct();
	//TODO: A팀 B팀 각각에 대한 텍스트블럭을 찾아옵니다.

	if (!ElementClass) UE_LOG(LogInit, Error, TEXT("UOccupationScoreBoard::ElementClass was nullptr!"));
}
