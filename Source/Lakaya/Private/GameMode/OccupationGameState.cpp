// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/OccupationGameState.h"

#include "GameMode/OccupationGameMode.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Net/UnrealNetwork.h"

void AOccupationGameState::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_GameTimeCheck, this, &AOccupationGameState::EndTimeCheck, 1.0f, true);
}

void AOccupationGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AOccupationGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOccupationGameState, NumPlayers);
	DOREPLIFETIME(AOccupationGameState, CurrentGameState);

	DOREPLIFETIME(AOccupationGameState, ATeamScore);
	DOREPLIFETIME(AOccupationGameState, BTeamScore);

	DOREPLIFETIME(AOccupationGameState, StartTime);
	DOREPLIFETIME(AOccupationGameState, MatchEndingTime);

	DOREPLIFETIME(AOccupationGameState, CurrentOccupationWinner);
}

void AOccupationGameState::SetNumPlayers(uint8 NewNumPlayers)
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
			GetWorldTimerManager().SetTimer(TimerHandle_AteamScoreIncrease, this, &AOccupationGameState::SetATeamScore, 1.0f, true);
			GetWorldTimerManager().SetTimer(TimerHandle_BteamScoreIncrease, this, &AOccupationGameState::SetBTeamScore, 1.0f, true);
			OnRep_GameState();
		}
	}
}

void AOccupationGameState::SetOccupationWinner(EOccupationWinner NewWinner)
{
	if (CurrentGameState == EOccupationGameState::Finish)
	{
		CurrentOccupationWinner = NewWinner;
		OnRep_OccupationWinner();
	}
}

void AOccupationGameState::SetATeamScore()
{
	if (CurrentGameState == EOccupationGameState::Progress)
	{
		if (GetATeamScore() <= MaxScore)
		{
			ATeamScore += (Standard) * GetATeamObjectNum();
			OnRep_ATeamScore();
		}
	}
}

void AOccupationGameState::SetBTeamScore()
{
	if (CurrentGameState == EOccupationGameState::Progress)
	{
		if (GetBTeamScore() <= MaxScore)
		{
			BTeamScore += (Standard) * GetBTeamObjectNum();
			OnRep_BTeamScore();
		}
	}
}

void AOccupationGameState::AddATeamObjectNum()
{
	ATeamObjectNum += 1;
}

void AOccupationGameState::AddBTeamObjectNum()
{
	BTeamObjectNum += 1;
}

void AOccupationGameState::SubATeamObjectNum()
{
	if (ATeamObjectNum > 0)
		ATeamObjectNum -= 1;
}

void AOccupationGameState::SubBTeamObjectNum()
{
	if (BTeamObjectNum > 0)
		BTeamObjectNum -= 1;
}

int32 AOccupationGameState::GetPlayerIndex(APlayerState* PlayerState) const
{
	// 플레이어의 인덱스를 찾을 수 없으면 -1를 반환합니다.
	int32 Index = -1;

	// PlayerArray에서 플레이어의 APlayerState 객체를 찾습니다.
	int32 IntNumPlayers = PlayerArray.Num();
	for (int32 i = 0; i < IntNumPlayers; i++)
	{
		if (PlayerArray[i] == PlayerState)
		{
			Index = i;
			break;
		}
	}

	return Index;
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

void AOccupationGameState::EndTimeCheck()
{
	if ((GetRemainMatchTime() <= 0 || (GetATeamScore() >= MaxScore || GetBTeamScore() >= MaxScore)) && CurrentGameState == EOccupationGameState::Progress)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_GameTimeCheck);

		auto GameMode = Cast<AOccupationGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("GameMode is null."));
			return;
		}
		
		GameMode->EndMatch();
	}
}

void AOccupationGameState::OnRep_NumPlayers()
{
	OnOccupationChangeJoinedPlayers.Broadcast(GetNumPlayers());
}

void AOccupationGameState::OnRep_GameState()
{
	OnOccupationChangeGameState.Broadcast(CurrentGameState);
}

void AOccupationGameState::OnRep_ATeamScore()
{
	OnOccupationChangeATeamScore.Broadcast(ATeamScore);
}

void AOccupationGameState::OnRep_BTeamScore()
{
	OnOccupationChangeBTeamScore.Broadcast(BTeamScore);
}

void AOccupationGameState::OnRep_OccupationWinner()
{
	OnOccupationChangeOccupationWinner.Broadcast(CurrentOccupationWinner);
}