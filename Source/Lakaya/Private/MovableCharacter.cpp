// Fill out your copyright notice in the Description page of Project Settings.


#include "MovableCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMovableCharacter::AMovableCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	RunMultiplier = 2.0;
	InteractionRange = 100;

	// In a dedicated server, the following logic is not necessary.
	if (IsRunningDedicatedServer()) return;

	// Character must look at the camera is looking at
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = bUseControllerRotationRoll = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

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

	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> InteractionContextFinder(
		TEXT("InputMappingContext'/Game/Dev/Yongwoo/Input/IC_InteractionControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> InteractionStartFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_InteractionStart'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> InteractionStopFinder(
		TEXT("InputAction'/Game/Dev/Yongwoo/Input/IA_InteractionStop'"));

	if (ContextFinder.Succeeded()) BasicControlContext = ContextFinder.Object;
	if (MoveFinder.Succeeded()) MoveAction = MoveFinder.Object;
	if (LookFinder.Succeeded()) LookAction = LookFinder.Object;
	if (JumpFinder.Succeeded()) JumpAction = JumpFinder.Object;
	if (CrouchFinder.Succeeded()) CrouchAction = CrouchFinder.Object;
	if (UnCrouchFinder.Succeeded()) UnCrouchAction = UnCrouchFinder.Object;
	if (RunFinder.Succeeded()) RunAction = RunFinder.Object;
	if (StopFinder.Succeeded()) StopRunningAction = StopFinder.Object;
	if (InteractionContextFinder.Succeeded()) InteractionContext = InteractionContextFinder.Object;
	if (InteractionStartFinder.Succeeded()) InteractionStartAction = InteractionStartFinder.Object;
	if (InteractionStopFinder.Succeeded()) InteractionStopAction = InteractionStopFinder.Object;
}

void AMovableCharacter::BeginPlay()
{
	Super::BeginPlay();

	InteractableCount = 0;
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
		if (InputSystem.IsValid()) InputSystem->AddMappingContext(BasicControlContext, BasicContextPriority);
	}
}

void AMovableCharacter::UnPossessed()
{
	Super::UnPossessed();

	if (InputSystem.IsValid()) InputSystem->RemoveMappingContext(BasicControlContext);
}

void AMovableCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// Add interaction context when overlapped by trigger
	if (!InputSystem.IsValid() || !OtherActor->ActorHasTag(TEXT("Interactable"))) return;
	++InteractableCount;
	if (!InputSystem->HasMappingContext(InteractionContext))
		InputSystem->AddMappingContext(InteractionContext, InteractionPriority);
}

void AMovableCharacter::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	// Remove interaction context when far away from triggers
	if (!InputSystem.IsValid() || !OtherActor->ActorHasTag(TEXT("Interactable"))) return;
	--InteractableCount;
	if (InteractableCount == 0) InputSystem->RemoveMappingContext(InteractionContext);
}

// Called to bind functionality to input
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
		CastedComponent->BindAction(InteractionStartAction, ETriggerEvent::Triggered, this,
		                            &AMovableCharacter::InteractionStart);
		CastedComponent->BindAction(InteractionStopAction, ETriggerEvent::Triggered, this,
		                            &AMovableCharacter::InteractionStop);
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

void AMovableCharacter::InteractionStart(const FInputActionValue& Value)
{
	FHitResult HitResult;
	const auto Location = SpringArm->GetComponentLocation();
	if (GetWorld()->LineTraceSingleByObjectType(HitResult, Location,
	                                            Location + SpringArm->GetForwardVector() * InteractionRange,
	                                            FCollisionObjectQueryParams::AllObjects))
	{
		InteractingActor = HitResult.GetActor();
		//TODO: 해당 액터에 대해 상호작용을 수행합니다.
	}
}

void AMovableCharacter::InteractionStop(const FInputActionValue& Value)
{
	if (InteractingActor.IsValid())
	{
		//TODO: 현재 상호작용중인 액터와 상호작용을 중지합니다.
	}
	InteractingActor.Reset();
}
