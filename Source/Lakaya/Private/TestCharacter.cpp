#include "TestCharacter.h"
#include "IndividualItem.h"

ATestCharacter::ATestCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	SetSpringArm();
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Mannequin(TEXT("/Game/Characters/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin"));
	if (SK_Mannequin.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Mannequin.Object);
	}

	// TODO : 애니메이션

	GetCharacterMovement()->JumpZVelocity = 400.0f;

	// TODO
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Character"));
}

void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ATestCharacter::SetSpringArm()
{
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 40.0f, 60.0f),
		FRotator(0.0f, 10.0f, 0.0f));
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = true;
	SpringArm->bDoCollisionTest = true;
	bUseControllerRotationRoll = false;
}

void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Action Mappings
	PlayerInputComponent->BindAction(TEXT("Spacebar"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	
	// Axis Mappings
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATestCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ATestCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATestCharacter::Turn);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &ATestCharacter::LookUp);
}

void ATestCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void ATestCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void ATestCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ATestCharacter::LookUp(float Value)
{
	AddControllerPitchInput(-Value);
}