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

	OccupationGameState->AddMaxPlayer();
	OccupationGameMode->NumPlayers++;
}

void AAiCharacterController::AiMove(FVector TargetLocation)
{
	APawn* ControlledPawn = GetPawn();

	AMovableCharacter* MovableCharacter =
		Cast<AMovableCharacter>(ControlledPawn);

	if (MovableCharacter && MovableCharacter->GetCharacterMovement())
	{
		MovableCharacter->GetCharacterMovement()->MaxWalkSpeed = 650.0f;
		MovableCharacter->GetCharacterMovement()->Velocity = FVector(600,600,0);
		MovableCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		
		const FRotator YawRotator(0, MovableCharacter->GetControlRotation().Yaw, 0);
		const FRotationMatrix Matrix(YawRotator);
		
		MovableCharacter->AddMovementInput(Matrix.GetUnitAxis(EAxis::X), 600);
		MovableCharacter->AddMovementInput(Matrix.GetUnitAxis(EAxis::Y), 600);
	}
}


