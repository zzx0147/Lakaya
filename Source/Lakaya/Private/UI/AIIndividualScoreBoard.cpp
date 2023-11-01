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
	const auto CharacterName = LakayaState->GetCharacterName().ToString();
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
	AIIndividualTeamBox->AddChildToVerticalBox(Element);
	Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Element->Individual_BackGround_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
	// LakayaState->OnCharacterNameChanged.AddLambda([Element](const ALakayaBasePlayerState* LakayaState, const FName& PlayerCharacterName)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("OnCharacterNameChanged."));
	//
	// 	if (PlayerCharacterName == TEXT("Rena"))
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("On_Rena"));
	// 		Element->Character_Rena_Image->SetVisibility(ESlateVisibility::HitTestInvisible);
	// 		Element->Character_Wazi_Image->SetVisibility(ESlateVisibility::Hidden);
	// 	}
	// 	if (PlayerCharacterName == TEXT("Wazi"))
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("On_Wazi"));
	// 		Element->Character_Wazi_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	// 		Element->Character_Rena_Image->SetVisibility(ESlateVisibility::Hidden);
	// 	}
	// });
	
	LakayaState->OnPlayerNameChanged.AddUObject(Element, &UScoreBoardElement::SetPlayerName);
	LakayaState->OnKillCountChanged.AddUObject(Element, &UScoreBoardElement::SetKillCount);
	LakayaState->OnDeathCountChanged.AddUObject(Element, &UScoreBoardElement::SetDeathCount);

	Element->SetPlayerName(LakayaState->GetPlayerName());
	Element->SetDeathCount(LakayaState->GetDeathCount());
	Element->SetKillCount(LakayaState->GetKillCount());
}