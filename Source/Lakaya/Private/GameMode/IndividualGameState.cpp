// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/IndividualGameState.h"

#include "Character/CollectorPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AIndividualGameState::BeginPlay()
{
	Super::BeginPlay();
}
	
void AIndividualGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIndividualGameState, NumPlayers);
	DOREPLIFETIME(AIndividualGameState, CurrentGameState);
	DOREPLIFETIME(AIndividualGameState, Min);
	DOREPLIFETIME(AIndividualGameState, Sec);
}

void AIndividualGameState::OnRep_NumPlayers()
{
	// 새로운 값으로 다른 클라이언트들에게 알림
	UE_LOG(LogTemp, Warning, TEXT("NumPlayers : %d"), NumPlayers);
	OnChangeJoinedPlayers.Broadcast(NumPlayers, GetMaxPlayers());
}

void AIndividualGameState::SetNumPlayers(int32 NewNumPlayers)
{
	NumPlayers = NewNumPlayers;
	OnRep_NumPlayers();
}

void AIndividualGameState::OnRep_GameState()
{
	OnChangeGameState.Broadcast(CurrentGameState);
}

void AIndividualGameState::OnRep_Min()
{
	OnChangeTime.Broadcast(Min, Sec);
}

void AIndividualGameState::OnRep_Sec()
{
	OnChangeTime.Broadcast(Min, Sec);
}

void AIndividualGameState::SetGameState(EGameState NewGameState)
{
	if (CurrentGameState != NewGameState)
	{
		CurrentGameState = NewGameState;
		OnRep_GameState();
	}
}