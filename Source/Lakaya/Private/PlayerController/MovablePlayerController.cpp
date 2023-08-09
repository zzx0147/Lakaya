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

	static const ConstructorHelpers::FObjectFinder<UInputAction> JumpFinder(
		TEXT("InputAction'/Game/Input/IA_Jump'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> CrouchFinder(
		TEXT("InputAction'/Game/Input/IA_Crouch'"));
	
	static const ConstructorHelpers::FObjectFinder<UInputAction> UnCrouchFinder(
		TEXT("InputAction'/Game/Input/IA_UnCrouch'"));
	
	if (ContextFinder.Succeeded()) MovementContext = ContextFinder.Object;
	if (MoveFinder.Succeeded()) MoveAction = MoveFinder.Object;
	if (LookFinder.Succeeded()) LookAction = LookFinder.Object;
	if (JumpFinder.Succeeded()) JumpAction = JumpFinder.Object;
	if (CrouchFinder.Succeeded()) CrouchAction = CrouchFinder.Object;
	if (UnCrouchFinder.Succeeded()) UnCrouchAction = UnCrouchFinder.Object;
}

void AMovablePlayerController::SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent)
{
	Super::SetupEnhancedInputComponent(EnhancedInputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Jump);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Crouch);
	EnhancedInputComponent->BindAction(UnCrouchAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::UnCrouch);
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

void AMovablePlayerController::Jump()
{
	if (const auto LocalCharacter = GetCharacter()) LocalCharacter->Jump();
}

void AMovablePlayerController::Crouch()
{
	if (const auto LocalCharacter = GetCharacter()) LocalCharacter->Crouch();
}

void AMovablePlayerController::UnCrouch()
{
	if (const auto LocalCharacter = GetCharacter()) LocalCharacter->UnCrouch();
}
