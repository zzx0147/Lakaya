// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/IndividualGameState.h"

#include "Character/CollectorPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AIndividualGameState::BeginPlay()
{
	Super::BeginPlay();

	// GameMode = Cast<AIndividualGameMode>(GetWorld()->GetAuthGameMode());
}
	
void AIndividualGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIndividualGameState, NumPlayers);
	DOREPLIFETIME(AIndividualGameState, CurrentGameState);
}

void AIndividualGameState::OnRep_NumPlayers()
{
	// 새로운 값으로 다른 클라이언트들에게 알림
	UE_LOG(LogTemp, Warning, TEXT("NumPlayers : %d"), NumPlayers);
}

void AIndividualGameState::SetNumPlayers(int32 NewNumPlayers)
{
	NumPlayers = NewNumPlayers;
	OnRep_NumPlayers();
}

void AIndividualGameState::OnRep_GameState()
{
	// GameMode = Cast<AIndividualGameMode>(GetWorld()->GetAuthGameMode());
	//
	// if (GameMode == nullptr)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("GameState_GameMode is null."));
	// 	return;
	// }
	//
	// if (GameMode->RegisteredPlayers.IsEmpty())
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("GameState_RegisteredPlayers is Empty"));
	// 	return;
	// }

	// UE_LOG(LogTemp, Warning, TEXT("PlayArray %d"), PlayerArray.Num());
	// for (APlayerState* State : PlayerArray)
	// {
	// 	APlayerController* PlayerController = Cast<APlayerController>(State->GetPlayerController());
	// 	if (PlayerController == nullptr)
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("GameState_PlayerController is null."));
	// 		return;
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("GameState_PlayerController is avaliable."));
	// 		return;
	// 	}
	// }
}

void AIndividualGameState::SetGameState(EGameState NewGameState)
{
	if (CurrentGameState != NewGameState)
	{
		CurrentGameState = NewGameState;
		OnRep_GameState();
	}
}