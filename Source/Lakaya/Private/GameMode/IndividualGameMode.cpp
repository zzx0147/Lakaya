// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/IndividualGameMode.h"

#include "Character/ArmedCharacter.h"
#include "Character/StatPlayerState.h"
#include "GameMode/IndividualGameState.h"
#include "PlayerController/BattlePlayerController.h"

AIndividualGameMode::AIndividualGameMode()
{
	DefaultPawnClass = AArmedCharacter::StaticClass();
	PlayerControllerClass = ABattlePlayerController::StaticClass();
	PlayerStateClass = AStatPlayerState::StaticClass();
	GameStateClass = AIndividualGameState::StaticClass();
}

void AIndividualGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AIndividualGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

bool AIndividualGameMode::ReadyToStartMatch_Implementation()
{
	return Super::ReadyToStartMatch_Implementation();
}

bool AIndividualGameMode::ReadyToEndMatch_Implementation()
{
	return Super::ReadyToEndMatch_Implementation();
}

void AIndividualGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void AIndividualGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}

void AIndividualGameMode::RespawnPlayer(AController* KilledController)
{
	Super::RespawnPlayer(KilledController);
}