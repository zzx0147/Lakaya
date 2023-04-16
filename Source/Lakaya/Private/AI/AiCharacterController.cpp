// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AiCharacterController.h"
#include "AI/AiCharacter.h"
#include "GameMode/OccupationGameMode.h"
#include "GameMode/OccupationGameState.h"

AAiCharacterController::AAiCharacterController() // 생성자
{
	bWantsPlayerState = true;
}

void AAiCharacterController::BeginPlay()
{
	Super::BeginPlay();

	AOccupationGameMode* OccupationGameMode = Cast<AOccupationGameMode>(GetWorld()->GetAuthGameMode());
	if (OccupationGameMode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AiCharacterController_GameMode is null."));
		return;
	}
	
	AOccupationGameState* OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameState_GameState is null."));
		return;
	}

	// OccupationGameState->AddMaxPlayer();
	// OccupationGameState->AddPlayerState(GetCharacter()->GetController()->PlayerState);
	// OccupationGameMode->NumPlayers++;
	// int32 CurrentPlayerNum = OccupationGameState->PlayerArray.Num();
	// OccupationGameState->SetNumPlayers(CurrentPlayerNum);
	// UE_LOG(LogTemp, Warning, TEXT("AiController BeginPlay."));
}

void AAiCharacterController::AiFireStart(AOccupationCharacter* OccuCharacter)
{
	ArmedCharacter = Cast<AArmedCharacter>(OccuCharacter);
   
	// if (OccuCharacter) OccuCharacter->FireStart();
	// else
	// {
	// 	UE_LOG(LogInit, Warning, TEXT("Error Ai Start Fire"))
	// }
}

void AAiCharacterController::AiFireStop(AOccupationCharacter* OccuCharacter)
{
	ArmedCharacter = Cast<AArmedCharacter>(OccuCharacter);
   
	// if (OccuCharacter) OccuCharacter->FireStop();
	// else
	// {
	// 	UE_LOG(LogInit, Warning, TEXT("Error Ai Stop Fire"))
	// }
}