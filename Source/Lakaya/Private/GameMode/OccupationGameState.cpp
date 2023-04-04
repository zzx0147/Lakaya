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
		OnRep_GameState();
		if (CurrentGameState == EOccupationGameState::Progress)
		{
			FTimerHandle GameTimerHandle;
			GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AOccupationGameState::SetMinSec, 1.0f, true);
		}
	}
}

void AOccupationGameState::SetMinSec()
{
	if (CurrentGameState == EOccupationGameState::Progress)
	{
		const float DeltaTime = GetWorld()->GetDeltaSeconds();
		const float TimeScale = 1.0f;
		
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

		Sec -= TimeScale * DeltaTime;
		// Sec -= 1;
		OnRep_Sec();
	}
}

void AOccupationGameState::OnRep_NumPlayers()
{
	// 새로운 값으로 다른 클라이언트들에게 알림
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
