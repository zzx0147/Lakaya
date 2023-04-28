// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/OccupationGameState.h"

#include "Net/UnrealNetwork.h"
#include "PlayerController/OccupationPlayerController.h"


void AOccupationGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOccupationGameState, NumPlayers);
	DOREPLIFETIME(AOccupationGameState, ATeamScore);
	DOREPLIFETIME(AOccupationGameState, BTeamScore);
	DOREPLIFETIME(AOccupationGameState, MatchStartTime);
	DOREPLIFETIME(AOccupationGameState, MatchEndingTime);
	DOREPLIFETIME(AOccupationGameState, CurrentOccupationWinner);
}

AOccupationGameState::AOccupationGameState()
{
	MaxPlayers = 2;
	MaxScore = 100.f;
	MatchDuration = 180.f;
}

void AOccupationGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	if (const auto Controller = GetWorld()->GetFirstPlayerController<AOccupationPlayerController>())
		Controller->OnMatchStart();
	else UE_LOG(LogInit, Error, TEXT("FirstPlayerController was not AOccupationPlayerController!"));
}

void AOccupationGameState::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	if (const auto Controller = GetWorld()->GetFirstPlayerController<AOccupationPlayerController>())
		Controller->OnMatchEnding();
	else UE_LOG(LogInit, Error, TEXT("FirstPlayerController was not AOccupationPlayerController!"));
}

void AOccupationGameState::SetNumPlayers(const uint8& NewNumPlayers)
{
	NumPlayers = NewNumPlayers;
	OnRep_NumPlayers();
}

void AOccupationGameState::NotifyKillCharacter_Implementation(AController* KilledController, AActor* KilledActor,
                                                              AController* EventInstigator, AActor* Causer)
{
	OnKillCharacterNotify.Broadcast(KilledController, KilledActor, EventInstigator, Causer);
}

void AOccupationGameState::SetOccupationWinner()
{
	CurrentOccupationWinner = ATeamScore > BTeamScore ? EPlayerTeam::A : EPlayerTeam::B;
	OnOccupationChangeOccupationWinner.Broadcast(CurrentOccupationWinner);
}

void AOccupationGameState::AddTeamScore(const EPlayerTeam& Team, const float& AdditiveScore)
{
	if (Team == EPlayerTeam::A) ATeamScore += AdditiveScore;
	else if (Team == EPlayerTeam::B) BTeamScore += AdditiveScore;
}

float AOccupationGameState::GetTeamScore(const EPlayerTeam& Team) const
{
	if (Team == EPlayerTeam::A) return ATeamScore;
	if (Team == EPlayerTeam::B) return BTeamScore;
	UE_LOG(LogScript, Warning, TEXT("Trying to GetTeamScore with not valid value! it was %d"), Team);
	return 0.f;
}

void AOccupationGameState::SetMatchTime()
{
	MatchStartTime = GetServerWorldTimeSeconds();
	MatchEndingTime = MatchStartTime + MatchDuration;
}

float AOccupationGameState::GetRemainMatchTime() const
{
	const auto Current = GetServerWorldTimeSeconds();
	return MatchEndingTime < Current ? 0 : MatchEndingTime - Current;
}

bool AOccupationGameState::IsSomeoneReachedMaxScore() const
{
	return ATeamScore >= MaxScore || BTeamScore >= MaxScore;
}

void AOccupationGameState::OnRep_NumPlayers()
{
	OnOccupationChangeJoinedPlayers.Broadcast(GetNumPlayers());
}

void AOccupationGameState::OnRep_ATeamScore()
{
	OnTeamScoreChanged.Broadcast(EPlayerTeam::A, ATeamScore);
}

void AOccupationGameState::OnRep_BTeamScore()
{
	OnTeamScoreChanged.Broadcast(EPlayerTeam::B, ATeamScore);
}

void AOccupationGameState::OnRep_OccupationWinner()
{
	OnOccupationChangeOccupationWinner.Broadcast(CurrentOccupationWinner);
}
