// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/OccupationGameState.h"

#include "Net/UnrealNetwork.h"


const float AOccupationGameState::MaxScore = 100.f;

void AOccupationGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOccupationGameState, NumPlayers);
	DOREPLIFETIME(AOccupationGameState, ATeamScore);
	DOREPLIFETIME(AOccupationGameState, BTeamScore);
	DOREPLIFETIME(AOccupationGameState, StartTime);
	DOREPLIFETIME(AOccupationGameState, MatchEndingTime);
	DOREPLIFETIME(AOccupationGameState, CurrentOccupationWinner);
}

void AOccupationGameState::SetNumPlayers(const uint8& NewNumPlayers)
{
	NumPlayers = NewNumPlayers;
	OnRep_NumPlayers();
}

void AOccupationGameState::SetOccupationWinner(const EPlayerTeamState& Winner)
{
	CurrentOccupationWinner = Winner;
	OnRep_OccupationWinner();
}

void AOccupationGameState::AddTeamScore(const EPlayerTeamState& Team, const float& AdditiveScore)
{
	if (Team == EPlayerTeamState::A) ATeamScore += AdditiveScore;
	else if (Team == EPlayerTeamState::B) BTeamScore += AdditiveScore;
}

const float& AOccupationGameState::GetTeamScore(const EPlayerTeamState& Team) const
{
	if (Team == EPlayerTeamState::A) return ATeamScore;
	else if (Team == EPlayerTeamState::B) return BTeamScore;
	UE_LOG(LogScript, Warning, TEXT("Trying to GetTeamScore with not valid value! it was %d"), Team);
	return 0.f;
}

void AOccupationGameState::SetMatchTime(const float& MatchTime)
{
	StartTime = GetServerWorldTimeSeconds();
	MatchEndingTime = StartTime + MatchTime;
}

float AOccupationGameState::GetRemainMatchTime()
{
	auto Current = GetServerWorldTimeSeconds();
	return MatchEndingTime < Current ? 0 : MatchEndingTime - Current;
}

void AOccupationGameState::OnRep_NumPlayers()
{
	OnOccupationChangeJoinedPlayers.Broadcast(GetNumPlayers());
}

void AOccupationGameState::OnRep_ATeamScore()
{
	OnTeamScoreChanged.Broadcast(EPlayerTeamState::A, ATeamScore);
}

void AOccupationGameState::OnRep_BTeamScore()
{
	OnTeamScoreChanged.Broadcast(EPlayerTeamState::B, ATeamScore);
}

void AOccupationGameState::OnRep_OccupationWinner()
{
	OnOccupationChangeOccupationWinner.Broadcast(CurrentOccupationWinner);
}
