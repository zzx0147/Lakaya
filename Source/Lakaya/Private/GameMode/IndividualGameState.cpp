// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/IndividualGameState.h"

#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "Character/CollectorPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AIndividualGameState::BeginPlay()
{
	Super::BeginPlay();

	// FTimerHandle GameTimerHandle;
	// GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AIndividualGameState::SetMinSec, 1.0f, true);
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
		if (CurrentGameState == EGameState::Progress)
		{
			FTimerHandle GameTimerHandle;
			GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AIndividualGameState::SetMinSec, 1.0f, true);
		}
	}
}

void AIndividualGameState::SetMinSec()
{
	if (CurrentGameState == EGameState::Progress)
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