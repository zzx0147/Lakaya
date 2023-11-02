// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/MovablePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/Character.h"

AMovablePlayerController::AMovablePlayerController()
{
	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Input/IC_CharacterControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> MoveFinder(
		TEXT("InputAction'/Game/Input/IA_Move'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> LookFinder(
		TEXT("InputAction'/Game/Input/IA_Look'"));

	if (ContextFinder.Succeeded()) MovementContext = ContextFinder.Object;
	if (MoveFinder.Succeeded()) MoveAction = MoveFinder.Object;
	if (LookFinder.Succeeded()) LookAction = LookFinder.Object;
}

void AMovablePlayerController::SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent)
{
	Super::SetupEnhancedInputComponent(EnhancedInputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Look);
}

void AMovablePlayerController::SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem)
{
	Super::SetupMappingContext(InputSubsystem);
	InputSubsystem->AddMappingContext(MovementContext, MovementContextPriority);
}

void AMovablePlayerController::Move(const FInputActionValue& Value)
{
	if (const auto LocalCharacter = GetCharacter())
	{
		const auto Vector = Value.Get<FVector2D>();
		const FRotationMatrix Matrix((FRotator(0, GetControlRotation().Yaw, 0)));

		LocalCharacter->AddMovementInput(Matrix.GetUnitAxis(EAxis::X), Vector.Y);
		LocalCharacter->AddMovementInput(Matrix.GetUnitAxis(EAxis::Y), Vector.X);
	}
}

void AMovablePlayerController::Look(const FInputActionValue& Value)
{
	const auto Vector = Value.Get<FVector2D>();
	AddYawInput(Vector.X);
	AddPitchInput(Vector.Y);
}
