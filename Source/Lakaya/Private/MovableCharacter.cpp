// Fill out your copyright notice in the Description page of Project Settings.


#include "MovableCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"

AMovableCharacter::AMovableCharacter()
{
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	RunMultiplier = 2.0;

	bUseControllerRotationYaw = bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = false;
	GetSpringArm()->bUsePawnControlRotation = true;

	// In a dedicated server, the following logic is not necessary.
	if (IsRunningDedicatedServer()) return;

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

	if (auto PlayerController = Cast<APlayerController>(Controller))
		if (auto LocalPlayer = PlayerController->GetLocalPlayer())
		{
			InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if (InputSystem.IsValid()) InputSystem->AddMappingContext(MovementContext, MovementContextPriority);
		}
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

bool AMovableCharacter::IsOwnedByLocalPlayer() const
{
	const auto PlayerController = Cast<APlayerController>(GetOwner());
	return PlayerController && PlayerController->IsLocalController();
}

void AMovableCharacter::RequestRun_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed *= RunMultiplier;
}

void AMovableCharacter::RequestStopRun_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed /= RunMultiplier;
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
	RequestRun();
}

void AMovableCharacter::StopRunning(const FInputActionValue& Value)
{
	RequestStopRun();
}
