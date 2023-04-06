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
	DOREPLIFETIME(AOccupationGameState, Min);
	DOREPLIFETIME(AOccupationGameState, Sec);
	
	DOREPLIFETIME(AOccupationGameState, CurrentOccupationObjectState);
	DOREPLIFETIME(AOccupationGameState, ATeamScore);
	DOREPLIFETIME(AOccupationGameState, BTeamScore);
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
			FTimerHandle GameTimerHandle;
			GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AOccupationGameState::SetMinSec, 1.0f, true);
			OnRep_GameState();
		}
	}
}

void AOccupationGameState::SetMinSec()
{
	if (CurrentGameState == EOccupationGameState::Progress)
	{
		if (Sec <= 0)
		{
			if (Min <= 0)
			{
				// TODO : 게임종료
				UE_LOG(LogTemp, Warning, TEXT("게임종료"));
				return;
			}

			Min -= 1;
			Sec = 60;
			OnRep_Min();
			OnRep_Sec();
		}
		Sec -= 1;
		OnRep_Sec();
	}
}

void AOccupationGameState::SetOccupationObject(EOccupationObjectState NewObjectState)
{
	if (CurrentOccupationObjectState != NewObjectState)
	{
		CurrentOccupationObjectState = NewObjectState;
		GetWorldTimerManager().SetTimer(TimerHandle_AteamScoreIncrease, this, &AOccupationGameState::SetATeamScore, 1.0f, true);
		GetWorldTimerManager().SetTimer(TimerHandle_BteamScoreIncrease, this, &AOccupationGameState::SetBTeamScore, 1.0f, true);
		
		if (CurrentOccupationObjectState == EOccupationObjectState::A || CurrentOccupationObjectState == EOccupationObjectState::B)
		return;
	}

	OnRep_OccupationObjectState();
}

void AOccupationGameState::SetATeamScore()
{
	if (CurrentGameState == EOccupationGameState::Progress)
	{
		if (CurrentOccupationObjectState != EOccupationObjectState::None)
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
		if (CurrentOccupationObjectState == EOccupationObjectState::B)
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

void AOccupationGameState::OnRep_NumPlayers()
{
	OnOccupationChangeJoinedPlayers.Broadcast(NumPlayers, GetMaxPlayers());
}

void AOccupationGameState::OnRep_GameState()
{
	OnOccupationChangeGameState.Broadcast(CurrentGameState);
}

void AOccupationGameState::OnRep_Min()
{
	OnOccupationChangeTime.Broadcast(Min, Sec);
}

void AOccupationGameState::OnRep_Sec()
{
	OnOccupationChangeTime.Broadcast(Min, Sec);
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
