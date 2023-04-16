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

	static const ConstructorHelpers::FObjectFinder<UInputAction> CrouchFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_Crouch'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> UnCrouchFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_UnCrouch'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> RunFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_Run'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> StopFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_StopRunning'"));

	if (ContextFinder.Succeeded()) MovementContext = ContextFinder.Object;
	if (MoveFinder.Succeeded()) MoveAction = MoveFinder.Object;
	if (LookFinder.Succeeded()) LookAction = LookFinder.Object;
	if (JumpFinder.Succeeded()) JumpAction = JumpFinder.Object;
	if (CrouchFinder.Succeeded()) CrouchAction = CrouchFinder.Object;
	if (UnCrouchFinder.Succeeded()) UnCrouchAction = UnCrouchFinder.Object;
	if (RunFinder.Succeeded()) RunAction = RunFinder.Object;
	if (StopFinder.Succeeded()) StopRunAction = StopFinder.Object;
}

void AMovablePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	const auto CastedComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!CastedComponent) UE_LOG(LogInit, Fatal, TEXT("InputComponent was not UEnhancedInputComponent!"));
	SetupEnhancedInputComponent(CastedComponent);

	const auto InputSubsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSubsystem) UE_LOG(LogInit, Fatal, TEXT("UEnhancedInputLocalPlayerSubsystem was not exist!"));
	SetupMappingContext(InputSubsystem);
}

void AMovablePlayerController::BeginPlay()
{
	Super::BeginPlay();
	OnPossessedPawnChanged.AddUniqueDynamic(this, &AMovablePlayerController::OnPossessedPawnChangedCallback);
}

void AMovablePlayerController::SetupEnhancedInputComponent(UEnhancedInputComponent* const& EnhancedInputComponent)
{
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Jump);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Crouch);
	EnhancedInputComponent->BindAction(UnCrouchAction, ETriggerEvent::Triggered, this,
	                                   &AMovablePlayerController::UnCrouch);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AMovablePlayerController::Run);
	EnhancedInputComponent->BindAction(StopRunAction, ETriggerEvent::Triggered, this,
	                                   &AMovablePlayerController::StopRun);
}

void AMovablePlayerController::SetupMappingContext(UEnhancedInputLocalPlayerSubsystem* const& InputSubsystem)
{
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

void AMovablePlayerController::Crouch(const FInputActionValue& Value)
{
	if (MovableCharacter.IsValid()) MovableCharacter->Crouch();
}

void AMovablePlayerController::UnCrouch(const FInputActionValue& Value)
{
	if (MovableCharacter.IsValid()) MovableCharacter->UnCrouch();
}

void AMovablePlayerController::Run(const FInputActionValue& Value)
{
	if (MovableCharacter.IsValid()) MovableCharacter->Run();
}

void AMovablePlayerController::StopRun(const FInputActionValue& Value)
{
	if (MovableCharacter.IsValid()) MovableCharacter->StopRun();
}

void AMovablePlayerController::Jump(const FInputActionValue& Value)
{
	if (MovableCharacter.IsValid()) MovableCharacter->Jump();
}