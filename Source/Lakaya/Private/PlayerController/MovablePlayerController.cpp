// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/MovablePlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Character/MovableCharacter.h"
#include "GameFramework/Character.h"

AMovablePlayerController::AMovablePlayerController()
{
	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Dev/Yongwoo/Input/IC_CharacterControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> MoveFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_Move'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> LookFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_Look'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> JumpFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_Jump'"));

	if (ContextFinder.Succeeded()) MovementContext = ContextFinder.Object;
	if (MoveFinder.Succeeded()) MoveAction = MoveFinder.Object;
	if (LookFinder.Succeeded()) LookAction = LookFinder.Object;
	if (JumpFinder.Succeeded()) JumpAction = JumpFinder.Object;
}

void AMovablePlayerController::SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent)
{
	Super::SetupEnhancedInputComponent(EnhancedInputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Jump);
}

void AMovablePlayerController::SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem)
{
	Super::SetupMappingContext(InputSubsystem);
	InputSubsystem->AddMappingContext(MovementContext, MovementContextPriority);
}

void AMovablePlayerController::OnPossessedPawnChangedCallback(APawn* ArgOldPawn, APawn* NewPawn)
{
	MovableCharacter = Cast<AMovableCharacter>(NewPawn);
	if (!MovableCharacter.IsValid()) UE_LOG(LogInit, Warning, TEXT("Possessed pawn was not AMovableCharacter"));
}

void AMovablePlayerController::Move(const FInputActionValue& Value)
{
	if (!MovableCharacter.IsValid()) return;
	const auto Vector = Value.Get<FVector2D>();
	const FRotationMatrix Matrix((FRotator(0, GetControlRotation().Yaw, 0)));

	MovableCharacter->AddMovementInput(Matrix.GetUnitAxis(EAxis::X), Vector.Y);
	MovableCharacter->AddMovementInput(Matrix.GetUnitAxis(EAxis::Y), Vector.X);
}

void AMovablePlayerController::Look(const FInputActionValue& Value)
{
	const auto Vector = Value.Get<FVector2D>();
	AddYawInput(Vector.X);
	AddPitchInput(Vector.Y);
}

void AMovablePlayerController::Jump(const FInputActionValue& Value)
{
	if (MovableCharacter.IsValid()) MovableCharacter->Jump();
}
