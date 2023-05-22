// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OccupationScoreBoard.h"

#include "Character/LakayaBasePlayerState.h"
#include "Components/VerticalBox.h"
#include "UI/ScoreBoardElement.h"

UOccupationScoreBoard::UOccupationScoreBoard(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ElementClass = UScoreBoardElement::StaticClass();
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

void UOccupationScoreBoard::RegisterPlayer(APlayerState* PlayerState)
{
	Super::RegisterPlayer(PlayerState);

	const auto LakayaState = Cast<ALakayaBasePlayerState>(PlayerState);
	const auto CharacterName = LakayaState->GetCharacterName().ToString();
	const auto Element = CreateWidget<UScoreBoardElement>(this, ElementClass);
	if (!LakayaState || !Element) return;
	UE_LOG(LogScript, Log, TEXT("UOccupationScoreBoard::RegisterPlayer Called"));

	switch (LakayaState->GetTeam())
	{
	case EPlayerTeam::A:
		if (ATeamBox.IsValid())
		{
			// TODO : 실행되지 않는 코드입니다.
			// ATeamBox->AddChildToVerticalBox(Element);
			// Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			
			// Element->Anti_BackGround_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			//
			// if (LakayaState->GetCharacterName().ToString() == "Rena")
			// {
			// 	Element->Character_Rena_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			// }
			//
			// if (LakayaState->GetCharacterName().ToString() == "Wazi")
			// {
			// 	Element->Character_Wazi_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			// }
		}
		break;
	case EPlayerTeam::B:
		if (BTeamBox.IsValid())
		{
			// TODO : 실행되지 않는 코드입니다.
			// BTeamBox->AddChildToVerticalBox(Element);
			// Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			
			// Element->Pro_BackGround_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			//
			// if (LakayaState->GetCharacterName().ToString() == "Rena")
			// {
			// 	Element->Character_Rena_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			// }
			//
			// if (LakayaState->GetCharacterName().ToString() == "Wazi")
			// {
			// 	Element->Character_Wazi_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			// }
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
			if (BTeamBox.IsValid())
				BTeamBox->RemoveChild(Element);
			if (ATeamBox.IsValid() && !ATeamBox->HasChild(Element))
				ATeamBox->AddChildToVerticalBox(Element);
			
			Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			Element->Anti_BackGround_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		// B팀으로 변경되면 A팀의 버티컬 박스에서 엘리먼트를 제거하고 B팀 버티컬 박스에 엘리먼트를 추가합니다.
		else if (ArgTeam == EPlayerTeam::B)
		{
			if (ATeamBox.IsValid())
				ATeamBox->RemoveChild(Element);
			if (BTeamBox.IsValid() && !BTeamBox->HasChild(Element))
				BTeamBox->AddChildToVerticalBox(Element);
			
			Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			Element->Pro_BackGround_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else UE_LOG(LogScript, Error, TEXT("Invalid team value on OnTeamChanged lambda handler!"));
	});
	
	LakayaState->OnDeathCountChanged.AddUObject(Element, &UScoreBoardElement::SetDeathCount);
	LakayaState->OnKillCountChanged.AddUObject(Element, &UScoreBoardElement::SetKillCount);
	LakayaState->OnPlayerNameChanged.AddUObject(Element, &UScoreBoardElement::SetPlayerName);

	Element->SetPlayerName(LakayaState->GetPlayerName());
	Element->SetDeathCount(LakayaState->GetDeathCount());
	Element->SetKillCount(LakayaState->GetKillCount());
}