// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AiCharacterController.h"
#include "AI/AiCharacter.h"
#include "Character/OccupationCharacter.h"

AAiCharacterController::AAiCharacterController() // 생성자
{
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

void AAiCharacterController::AiFireStart(AOccupationCharacter* OccuCharacter)
{
	ArmedCharacter = Cast<AArmedCharacter>(OccuCharacter);
	
	if (OccuCharacter) OccuCharacter->FireStart();
	else
	{
		UE_LOG(LogInit, Warning, TEXT("Ai nonenone Fire"))
	}
}

void AAiCharacterController::AiFireStop()
{
	if (ArmedCharacter.IsValid()) ArmedCharacter->FireStop();
}


