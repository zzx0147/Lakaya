// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HealthComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABasePlayerCharacter::ABasePlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	RunMultiplier = 2.0;

	// Character must look at the camera is looking at
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = bUseControllerRotationRoll = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	static const ConstructorHelpers::FObjectFinder<UInputMappingContext> ContextFinder(
		TEXT("InputMappingContext'/Game/Yongwoo/Input/IC_CharacterControl'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> MoveFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_Move'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> LookFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_Look'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> JumpFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_Jump'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> CrouchFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_Crouch'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> UnCrouchFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_UnCrouch'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> RunFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_Run'"));

	static const ConstructorHelpers::FObjectFinder<UInputAction> StopFinder(
		TEXT("InputAction'/Game/Yongwoo/Input/IA_StopRunning'"));

	if (ContextFinder.Succeeded()) BasicControlContext = ContextFinder.Object;
	if (MoveFinder.Succeeded()) MoveAction = MoveFinder.Object;
	if (LookFinder.Succeeded()) LookAction = LookFinder.Object;
	if (JumpFinder.Succeeded()) JumpAction = JumpFinder.Object;
	if (CrouchFinder.Succeeded()) CrouchAction = CrouchFinder.Object;
	if (UnCrouchFinder.Succeeded()) UnCrouchAction = UnCrouchFinder.Object;
	if (RunFinder.Succeeded()) RunAction = RunFinder.Object;
	if (StopFinder.Succeeded()) StopRunningAction = StopFinder.Object;
}

void ABasePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->OnDead.AddLambda([this] { Destroy(); });
	HealthComponent->OnHealthChanged.AddLambda([](const float& Health, const float& Delta)
	{
		// Debug code
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White,
		                                 FString::Printf(TEXT("Health : %f, Delta : %f"), Health, Delta));
	});
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

	// Debug necessary. want to remove InputMappingContext from subsystem when unpossess
	if (const auto PlayerController = Cast<APlayerController>(Controller))
		if (const auto InputSystem = PlayerController->GetLocalPlayer()->GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>())
			InputSystem->RemoveMappingContext(BasicControlContext);
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
		InputComponent->BindAction(StopRunningAction, ETriggerEvent::Triggered, this,
		                           &ABasePlayerCharacter::StopRunning);
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
	// This can causing problem when change RunMultiplier until running
	GetCharacterMovement()->MaxWalkSpeed /= RunMultiplier;
}
