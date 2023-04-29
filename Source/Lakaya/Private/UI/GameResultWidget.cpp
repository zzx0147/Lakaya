// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameResultWidget.h"

#include "Character/OccupationPlayerState.h"
#include "GameFramework/Character.h"
#include "GameMode/OccupationGameState.h"

bool UGameResultWidget::OnMatchEnding()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	return true;
}

void UGameResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameResultWidgetText = Cast<UTextBlock>(GetWidgetFromName(TEXT("GameResultWidgetText")));
	if (GameResultWidgetText == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_JoinedPlayerText is null."));
		return;
	}

	OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LoadingWidget_GameMode is null."));
		return;
	}

	OccupationGameState->OnOccupationChangeOccupationWinner.AddUObject(this, &UGameResultWidget::OnChangeWinner);
}

void UGameResultWidget::OnChangeWinner(const EPlayerTeam& NewWinner)
{
	SetVisibility(ESlateVisibility::Visible);

	AOccupationPlayerState* OccupationPlayerState = Cast<AOccupationPlayerState>(GetOwningPlayer()->GetCharacter()->GetController());
	if (OccupationPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationPlayerState is null."));
		return;
	}
	
	FString WinnerString;
	switch (OccupationGameState->GetOccupationWinner())
	{
	case EPlayerTeam::None:
		WinnerString = FString(TEXT("Undecided"));
		break;
	case EPlayerTeam::A:
		WinnerString = FString(TEXT("A"));
		if (OccupationPlayerState->GetPlayerTeamState() == EPlayerTeam::A) GameResultWidgetText->SetText(FText::FromString(FString::Printf(TEXT("승리!"))));
		else GameResultWidgetText->SetText(FText::FromString(FString::Printf(TEXT("패배."))));
		break;
	case EPlayerTeam::B:
		WinnerString = FString(TEXT("B"));
		if (OccupationPlayerState->GetPlayerTeamState() == EPlayerTeam::A) GameResultWidgetText->SetText(FText::FromString(FString::Printf(TEXT("패배."))));
		else GameResultWidgetText->SetText(FText::FromString(FString::Printf(TEXT("승리!"))));
		break;
	default:
		WinnerString = FString(TEXT("Unknown"));
		break;
	}
}
