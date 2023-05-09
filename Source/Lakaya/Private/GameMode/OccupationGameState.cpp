// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/OccupationGameState.h"

#include "Net/UnrealNetwork.h"


void AOccupationGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOccupationGameState, ATeamScore);
	DOREPLIFETIME(AOccupationGameState, BTeamScore);
	DOREPLIFETIME(AOccupationGameState, CurrentOccupationWinner);
}

AOccupationGameState::AOccupationGameState()
{
	MaxScore = 100.f;
	MatchDuration = 180.f;
}

void AOccupationGameState::SetNumPlayers(const uint8& NewNumPlayers)
{
	NumPlayers = NewNumPlayers;
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

	UE_LOG(LogTemp, Warning, TEXT("ATeamScore : %f"), ATeamScore);
	UE_LOG(LogTemp, Warning, TEXT("BTeamScore : %f"), BTeamScore);
}

float AOccupationGameState::GetTeamScore(const EPlayerTeam& Team) const
{
	if (Team == EPlayerTeam::A) return ATeamScore;
	if (Team == EPlayerTeam::B) return BTeamScore;
	UE_LOG(LogScript, Warning, TEXT("Trying to GetTeamScore with not valid value! it was %d"), Team);
	return 0.f;
}

bool AOccupationGameState::IsSomeoneReachedMaxScore() const
{
	return ATeamScore >= MaxScore || BTeamScore >= MaxScore;
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
