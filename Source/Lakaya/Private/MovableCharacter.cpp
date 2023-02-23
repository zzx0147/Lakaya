// Fill out your copyright notice in the Description page of Project Settings.


#include "MovableCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"

AMovableCharacter::AMovableCharacter()
{
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	RunMultiplier = 2.0;

	// In a dedicated server, the following logic is not necessary.
	if (IsRunningDedicatedServer()) return;

	// Character must look at the camera is looking at
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = bUseControllerRotationRoll = false;
	GetSpringArm()->bUsePawnControlRotation = true;

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
	if (StopFinder.Succeeded()) StopRunningAction = StopFinder.Object;
}

void AMovableCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMovableCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsRunningDedicatedServer()) return;

	if (const auto PlayerController = Cast<APlayerController>(NewController))
	{
		// If It's not listen server, set role as AutonomousProxy
		if (!HasAuthority()) SetRole(ROLE_AutonomousProxy);
		InputSystem = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (InputSystem.IsValid()) InputSystem->AddMappingContext(MovementContext, MovementContextPriority);
	}
}

void AMovableCharacter::UnPossessed()
{
	Super::UnPossessed();

	if (InputSystem.IsValid()) InputSystem->RemoveMappingContext(MovementContext);
}

void AMovableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const auto CastedComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		CastedComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMovableCharacter::Move);
		CastedComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMovableCharacter::Look);
		CastedComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		CastedComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AMovableCharacter::Crouching);
		CastedComponent->BindAction(UnCrouchAction, ETriggerEvent::Triggered, this, &AMovableCharacter::UnCrouching);
		CastedComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AMovableCharacter::Run);
		CastedComponent->BindAction(StopRunningAction, ETriggerEvent::Triggered, this,
		                            &AMovableCharacter::StopRunning);
	}
}

void AMovableCharacter::Move(const FInputActionValue& Value)
{
	const auto Vector = Value.Get<FVector2D>();
	const FRotator YawRotator(0, Controller->GetControlRotation().Yaw, 0);
	const FRotationMatrix Matrix(YawRotator);

	AddMovementInput(Matrix.GetUnitAxis(EAxis::X), Vector.Y);
	AddMovementInput(Matrix.GetUnitAxis(EAxis::Y), Vector.X);
}

void AMovableCharacter::Look(const FInputActionValue& Value)
{
	const auto Vector = Value.Get<FVector2D>();
	AddControllerYawInput(Vector.X);
	AddControllerPitchInput(Vector.Y);
}

void AMovableCharacter::Crouching(const FInputActionValue& Value)
{
	Crouch();
}

void AMovableCharacter::UnCrouching(const FInputActionValue& Value)
{
	UnCrouch();
}

void AMovableCharacter::Run(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed *= RunMultiplier;
}

void AMovableCharacter::StopRunning(const FInputActionValue& Value)
{
	// This can causing problem when change RunMultiplier until running
	GetCharacterMovement()->MaxWalkSpeed /= RunMultiplier;
}
