// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/OccupationGameState.h"
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
	DOREPLIFETIME(AOccupationGameState, MaxPlayers);
	DOREPLIFETIME(AOccupationGameState, CurrentGameState);

	DOREPLIFETIME(AOccupationGameState, CurrentOccupationObjectState);
	DOREPLIFETIME(AOccupationGameState, ATeamScore);
	DOREPLIFETIME(AOccupationGameState, BTeamScore);

	DOREPLIFETIME(AOccupationGameState, ATeamObjectNum);
	DOREPLIFETIME(AOccupationGameState, BTeamObjectNum);

	DOREPLIFETIME(AOccupationGameState, StartTime);
	DOREPLIFETIME(AOccupationGameState, MatchEndingTime);

	DOREPLIFETIME(AOccupationGameState, CurrentOccupationWinner);
}

void AOccupationGameState::SetNumPlayers(int32 NewNumPlayers)
{
	NumPlayers = NewNumPlayers;
	OnRep_NumPlayers();
	// OnRep_MaxPlayers();
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
// <<<<<<< HEAD
		GetWorldTimerManager().SetTimer(TimerHandle_AteamScoreIncrease, this, &AOccupationGameState::SetATeamScore, 1.0f, true);
		GetWorldTimerManager().SetTimer(TimerHandle_BteamScoreIncrease, this, &AOccupationGameState::SetBTeamScore, 1.0f, true);
		
		if (CurrentOccupationObjectState == EOccupationObjectState::A || CurrentOccupationObjectState == EOccupationObjectState::B)
		return;
// =======
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

	OnRep_OccupationObjectState();
}

void AOccupationGameState::SetOccupationWinner(EOccupationWinner NewWinner)
{
	if (CurrentGameState == EOccupationGameState::Finish)
	{
		CurrentOccupationWinner = NewWinner;
		OnRep_OccupationWinner();
	}
}

void AOccupationGameState::AddMaxPlayer()
{
	MaxPlayers++;
	OnRep_MaxPlayers();
	// OnRep_NumPlayers();
}

void AOccupationGameState::SetATeamScore()
{
	if (CurrentGameState == EOccupationGameState::Progress)
	{
		if (CurrentOccupationObjectState != EOccupationObjectState::None && GetATeamScore() <= MaxScore)
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
		if (CurrentOccupationObjectState != EOccupationObjectState::None && GetBTeamScore() <= MaxScore)
		{
			BTeamScore += (Standard) * GetBTeamObjectNum();
			OnRep_BTeamScore();
		}
	}
}

void AOccupationGameState::AddATeamObjectNum()
{
	ATeamObjectNum += 1;
	OnRep_ATeamObjectNum();
}

void AOccupationGameState::AddBTeamObjectNum()
{
	BTeamObjectNum += 1;
	OnRep_BTeamObjectNum();
}

void AOccupationGameState::SubATeamObjectNum()
{
	if (ATeamObjectNum > 0)
		ATeamObjectNum -= 1;
	else return;
	
	OnRep_ATeamObjectNum();
}

void AOccupationGameState::SubBTeamObjectNum()
{
	if (BTeamObjectNum > 0)
		BTeamObjectNum -= 1;
	else return;
	
	OnRep_BTeamObjectNum();
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

		// UE_LOG(LogTemp, Warning, TEXT("게임이 종료 되었습니다."));
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
	OnOccupationChangeJoinedPlayers.Broadcast(GetNumPlayers(), GetMaxPlayers());
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

void AOccupationGameState::OnRep_ATeamObjectNum()
{
	OnOccupationChangeATeamObjectNum.Broadcast(ATeamObjectNum);
}

void AOccupationGameState::OnRep_BTeamObjectNum()
{
	OnOccupationChangeBTeamObjectNum.Broadcast(BTeamObjectNum);
}

void AOccupationGameState::OnRep_OccupationWinner()
{
	OnOccupationChangeOccupationWinner.Broadcast(CurrentOccupationWinner);
}

void AOccupationGameState::OnRep_MaxPlayers()
{
	OnOccupationChangeMaxPlayers.Broadcast(GetNumPlayers(), GetMaxPlayers());
}