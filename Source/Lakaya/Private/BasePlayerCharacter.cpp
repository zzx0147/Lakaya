// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABasePlayerCharacter::ABasePlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABasePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABasePlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (const auto PlayerController = Cast<APlayerController>(NewController))
		if (const auto InputSystem = PlayerController->GetLocalPlayer()->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>())
			InputSystem->AddMappingContext(BasicControlContext, ContextPriority);
}

void ABasePlayerCharacter::UnPossessed()
{
	Super::UnPossessed();

	// if(const auto PlayerController = Cast<APlayerController>(Controller))
	// 	if(const auto InputSystem = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	// 		InputSystem->RemoveMappingContext(BasicControlContext.LoadSynchronous());
}

// Called every frame
void ABasePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABasePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (const auto InputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		InputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasePlayerCharacter::Move);
		InputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABasePlayerCharacter::Look);
		InputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		InputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ABasePlayerCharacter::Crouching);
		InputComponent->BindAction(UnCrouchAction, ETriggerEvent::Triggered, this, &ABasePlayerCharacter::UnCrouching);
		InputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &ABasePlayerCharacter::Run);
		InputComponent->BindAction(StopRunningAction,ETriggerEvent::Triggered,this,&ABasePlayerCharacter::StopRunning);
	}
}

void ABasePlayerCharacter::Move(const FInputActionValue& Value)
{
	const auto Vector = Value.Get<FVector2D>();
	const FRotator YawRotator(0, Controller->GetControlRotation().Yaw, 0);
	const FRotationMatrix Matrix(YawRotator);

	AddMovementInput(Matrix.GetUnitAxis(EAxis::X), Vector.Y);
	AddMovementInput(Matrix.GetUnitAxis(EAxis::Y), Vector.X);
}

void ABasePlayerCharacter::Look(const FInputActionValue& Value)
{
	const auto Vector = Value.Get<FVector2D>();
	AddControllerYawInput(Vector.X);
	AddControllerPitchInput(Vector.Y);
}

void ABasePlayerCharacter::Crouching(const FInputActionValue& Value)
{
	Crouch();
}

void ABasePlayerCharacter::UnCrouching(const FInputActionValue& Value)
{
	UnCrouch();
}

void ABasePlayerCharacter::Run(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed *= RunMultiplier;
}

void ABasePlayerCharacter::StopRunning(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed /= RunMultiplier;
}
