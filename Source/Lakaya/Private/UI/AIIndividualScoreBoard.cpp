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
	AIIndividualTeamBox->AddChildToVerticalBox(Element);
	Element->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Element->Individual_BackGround_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
	LakayaState->OnPlayerNameChanged.AddUObject(Element, &UScoreBoardElement::SetPlayerName);
	LakayaState->OnKillCountChanged.AddUObject(Element, &UScoreBoardElement::SetKillCount);
	LakayaState->OnDeathCountChanged.AddUObject(Element, &UScoreBoardElement::SetDeathCount);

	LakayaState->OnKillCountChanged.AddUObject(this, &UAIIndividualScoreBoard::OnKillCountChanged);
	
	Element->SetPlayerName(LakayaState->GetPlayerName());
	Element->SetDeathCount(LakayaState->GetDeathCount());
	Element->SetKillCount(LakayaState->GetKillCount());
	Element->IndividualKillCount = LakayaState->GetKillCount();
	
}

void UAIIndividualScoreBoard::OnKillCountChanged(const uint16& NewKillCount)
{
	




	
	// TArray<UWidget*> ChildWidgets = AIIndividualTeamBox->GetAllChildren();
	// TArray<UAIIndividualScoreBoardElement*> ScoreBoardElements;
	//
	// for (UWidget* ChildWidget : ChildWidgets)
	// {
	// 	UAIIndividualScoreBoardElement* ScoreBoardElement = Cast<UAIIndividualScoreBoardElement>(ChildWidget);
	// 	if (ScoreBoardElement)
	// 	{
	// 		ScoreBoardElements.Add(ScoreBoardElement);
	// 		ScoreBoardElement->SetIndividualRank(0); // 등수 초기화
	// 		UE_LOG(LogTemp, Error, TEXT("캐릭터 킬수 : %d"), ScoreBoardElement->GetKillCount());
	// 		UE_LOG(LogTemp, Error, TEXT("킬수 : %d"), NewKillCount);
	// 	}
	// }
	//
	// ScoreBoardElements.Sort([](const UAIIndividualScoreBoardElement& A, const UAIIndividualScoreBoardElement& B)
	// { return A.GetKillCount() > B.GetKillCount(); });
	//
	// int32 Rank = 1;
	// for (int32 Index = 0; Index < ScoreBoardElements.Num(); ++Index)
	// {
	// 	UAIIndividualScoreBoardElement* ScoreBoardElement = ScoreBoardElements[Index];
	// 	ScoreBoardElement->SetIndividualRank(Rank); // 등수 설정
	// 	Rank++;
	// 	
	// }
	//
	// AIIndividualTeamBox->ClearChildren();
	//
	// for (int32 i = 0; i < ScoreBoardElements.Num(); i++)
	// {
	// 	AIIndividualTeamBox->AddChild(ScoreBoardElements[i]);
	// }

	// for (UAIIndividualScoreBoardElement* ScoreBoardElement : ScoreBoardElements)
	// {
	// 	AIIndividualTeamBox->AddChild(ScoreBoardElement);
	// }
}

// void UAIIndividualScoreBoard::OnKillCountChanged(const uint16& NewKillCount)
// {
// 	TArray<UAIIndividualScoreBoardElement*> ScoreBoardElements;
//
// 	for (int32 Index = 0; Index < AIIndividualTeamBox->GetChildrenCount(); ++Index)
// 	{
// 		UWidget* ChildWidget = AIIndividualTeamBox->GetChildAt(Index);
// 		UAIIndividualScoreBoardElement* ScoreBoardElement = Cast<UAIIndividualScoreBoardElement>(ChildWidget);
// 		if (ScoreBoardElement)
// 		{
// 			ScoreBoardElements.Add(ScoreBoardElement);
// 			ScoreBoardElement->SetIndividualRank(0); // 등수 초기화
// 		}
// 	}
//
// 	ScoreBoardElements.Sort([](const UAIIndividualScoreBoardElement& A, const UAIIndividualScoreBoardElement& B) {
// 		return A.GetKillCount() > B.GetKillCount();
// 	});
//
// 	for (int32 Index = 0; Index < ScoreBoardElements.Num(); ++Index)
// 	{
// 		UAIIndividualScoreBoardElement* ScoreBoardElement = ScoreBoardElements[Index];
// 		ScoreBoardElement->SetIndividualRank(Index + 1); // 등수 설정
// 	}
//
// 	AIIndividualTeamBox->ClearChildren();
//
// 	for (UAIIndividualScoreBoardElement* ScoreBoardElement : ScoreBoardElements)
// 	{
// 		AIIndividualTeamBox->AddChild(ScoreBoardElement);
// 	}
// }
