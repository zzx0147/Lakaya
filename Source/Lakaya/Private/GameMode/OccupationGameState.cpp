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
		// const float DeltaTime = GetWorld()->GetDeltaSeconds();
		// const float TimeScale = 1.0f;
		
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

		// Sec -= TimeScale * DeltaTime;
		Sec -= 1;
		OnRep_Sec();
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
			
			GetWorldTimerManager().SetTimer(TimerHandle_AteamScoreIncrease, this, &AOccupationGameState::SetATeamScore, 1.0f, true);
		}
		else if (CurrentOccupationObjectState == EOccupationObjectState::B)
		{
			if (GetWorldTimerManager().IsTimerActive(TimerHandle_AteamScoreIncrease))
			{
				GetWorldTimerManager().ClearTimer(TimerHandle_AteamScoreIncrease);	
			}
			
			GetWorldTimerManager().SetTimer(TimerHandle_BteamScoreIncrease, this, &AOccupationGameState::SetBTeamScore, 1.0f, true);
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
			UE_LOG(LogTemp, Warning, TEXT("A팀 점수 획득. %d"), ATeamScore);
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
			UE_LOG(LogTemp, Warning, TEXT("B팀 점수 획득. %d"), BTeamScore);
		}
	}
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
	UE_LOG(LogTemp, Warning, TEXT("OnRep_ATeamScore"));
}

void AOccupationGameState::OnRep_BTeamScore()
{
	OnOccupationChangeBTeamScore.Broadcast(BTeamScore);
	UE_LOG(LogTemp, Warning, TEXT("OnRep_BTeamScore"));
}