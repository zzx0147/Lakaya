// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/IndividualGameState.h"

#include "Character/CollectorPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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
	// TArray<AActor*> FoundActors;
	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACollectorPlayerState::StaticClass(), FoundActors);

	// for (AActor* FoundActor : FoundActors)
	// {
		// ACollectorPlayerState* PlayerState = FoundActor->getplayer
	// }
	
	// for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	// {
	// 	APlayerController* PC = It->Get();
	// 	if (PC && PC->IsLocalController())
	// 	{
	// 		// 게임상태가 바뀔 때, 모든 클라이언트들의 위젯을 업데이트
	// 		UE_LOG(LogTemp, Warning, TEXT("CurrentGameState : %s"), CurrentGameState);
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