// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LakayaBaseGameState.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "UI/GameScoreBoardWidget.h"
#include "UI/LoadingWidget.h"

ALakayaBaseGameState::ALakayaBaseGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	MaximumPlayers = 6;
	PlayersNumber = 0;
}

void ALakayaBaseGameState::BeginPlay()
{
	Super::BeginPlay();
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		if (LoadingWidgetClass.Get() != nullptr)
		{
			LoadingWidget = CreateWidget<ULoadingWidget>(LocalController, LoadingWidgetClass);
			if (LoadingWidget != nullptr)
			{
				LoadingWidget->OnBeginPlay();
				LoadingWidget->SetMaximumPlayerNumber(MaximumPlayers);
				LoadingWidget->SetPlayerNumber(PlayerArray.Num());
				LoadingWidget->AddToViewport();
				LoadingWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}

		if (ScoreBoardClass)
		{
			ScoreBoard = CreateWidget<UGameScoreBoardWidget>(LocalController, ScoreBoardClass);
			if (ScoreBoard.IsValid())
			{
				ScoreBoard->AddToViewport();
				ScoreBoard->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	PlayersNumber = PlayerArray.Num();
}

void ALakayaBaseGameState::Tick(float DeltaTime)
{
	//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, *GetMatchState().ToString());
}

void ALakayaBaseGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	if (LoadingWidget != nullptr) LoadingWidget->SetPlayerNumber(PlayerArray.Num());
	if (ScoreBoard.IsValid()) ScoreBoard->RegisterPlayer(PlayerState);

	OnChangePlayerNumber.Broadcast(PlayerArray.Num());
	// PlayersNumber 멤버변수는 불필요해보임
	PlayersNumber = PlayerArray.Num();
	//if (PlayerArray.Num() >= MaximumPlayers)
}

void ALakayaBaseGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	if (LoadingWidget != nullptr) LoadingWidget->SetPlayerNumber(PlayerArray.Num());
	OnChangePlayerNumber.Broadcast(PlayerArray.Num());
	PlayersNumber = PlayerArray.Num();
}

void ALakayaBaseGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void ALakayaBaseGameState::HandleMatchIsCharacterSelect()
{
	LoadingWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ALakayaBaseGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();
	if (MatchState == MatchState::IsSelectCharacter)
	{
		HandleMatchIsCharacterSelect();
	}
}

void ALakayaBaseGameState::SetScoreBoardVisibility(const bool& Visible)
{
	if (ScoreBoard.IsValid())
		ScoreBoard->SetVisibility(Visible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}