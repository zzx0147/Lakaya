// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/OccupationGameState.h"
#include "Net/UnrealNetwork.h"

void AOccupationGameState::BeginPlay()
{
	Super::BeginPlay();
}

void AOccupationGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOccupationGameState, NumPlayers);
	DOREPLIFETIME(AOccupationGameState, CurrentGameState);

	DOREPLIFETIME(AOccupationGameState, CurrentOccupationObjectState);
	DOREPLIFETIME(AOccupationGameState, ATeamScore);
	DOREPLIFETIME(AOccupationGameState, BTeamScore);

	DOREPLIFETIME(AOccupationGameState, StartTime);
	DOREPLIFETIME(AOccupationGameState, MatchEndingTime);
}

void AOccupationGameState::SetNumPlayers(int32 NewNumPlayers)
{
	NumPlayers = NewNumPlayers;
	OnRep_NumPlayers();
}

void AOccupationGameState::SetGameState(EOccupationGameState NewGameState)
{
	if (CurrentGameState != NewGameState)
	{
		CurrentGameState = NewGameState;
		if (CurrentGameState == EOccupationGameState::Progress)
		{
			OnRep_GameState();
		}
	}
}

void AOccupationGameState::SetOccupationObject(EOccupationObjectState NewObjectState)
{
	if (CurrentOccupationObjectState != NewObjectState)
	{
		CurrentOccupationObjectState = NewObjectState;
		if (CurrentOccupationObjectState == EOccupationObjectState::A)
		{
			// TODO : 점수 올려 줘야 함
			if (GetWorldTimerManager().IsTimerActive(TimerHandle_BteamScoreIncrease))
			{
				GetWorldTimerManager().ClearTimer(TimerHandle_BteamScoreIncrease);
			}

			GetWorldTimerManager().SetTimer(TimerHandle_AteamScoreIncrease, this, &AOccupationGameState::SetATeamScore,
			                                1.0f, true);
		}
		else if (CurrentOccupationObjectState == EOccupationObjectState::B)
		{
			if (GetWorldTimerManager().IsTimerActive(TimerHandle_AteamScoreIncrease))
			{
				GetWorldTimerManager().ClearTimer(TimerHandle_AteamScoreIncrease);
			}

			GetWorldTimerManager().SetTimer(TimerHandle_BteamScoreIncrease, this, &AOccupationGameState::SetBTeamScore,
			                                1.0f, true);
		}

		OnRep_OccupationObjectState();
	}
}

void AOccupationGameState::SetATeamScore()
{
	if (CurrentGameState == EOccupationGameState::Progress)
	{
		if (CurrentOccupationObjectState == EOccupationObjectState::A)
		{
			// const float DeltaTime = GetWorld()->GetDeltaSeconds();
			// const float TimeScale = 1.0f;

			// ATeamScore += TimeScale * DeltaTime;
			ATeamScore += 1;
			OnRep_ATeamScore();
		}
	}
}

void AOccupationGameState::SetBTeamScore()
{
	if (CurrentGameState == EOccupationGameState::Progress)
	{
		if (CurrentOccupationObjectState == EOccupationObjectState::B)
		{
			// const float DeltaTime = GetWorld()->GetDeltaSeconds();
			// const float TimeScale = 1.0f;

			// BTeamScore += TimeScale * DeltaTime;;
			BTeamScore += 1;
			OnRep_BTeamScore();
		}
	}
}

void AOccupationGameState::OnMatchStarted(const float& MatchTime)
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
	OnOccupationChangeJoinedPlayers.Broadcast(NumPlayers, GetMaxPlayers());
}

void AOccupationGameState::OnRep_GameState()
{
	OnOccupationChangeGameState.Broadcast(CurrentGameState);
}

void AOccupationGameState::OnRep_OccupationObjectState()
{
	OnOccupationChangeObjectcState.Broadcast(CurrentOccupationObjectState);
}

void AOccupationGameState::OnRep_ATeamScore()
{
	OnOccupationChangeATeamScore.Broadcast(ATeamScore);
}

void AOccupationGameState::OnRep_BTeamScore()
{
	OnOccupationChangeBTeamScore.Broadcast(BTeamScore);
}
