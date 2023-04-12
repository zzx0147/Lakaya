// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ThirdPersonCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AThirdPersonCharacter::AThirdPersonCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCapsuleComponent()->SetCapsuleHalfHeight(GetCapsuleComponent()->GetUnscaledCapsuleRadius());
	bUseControllerRotationYaw = bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = false;
	YawClutch = false;

	bFixMeshTransform = true;
}

void AThirdPersonCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bFixMeshTransform)
	{
		auto Rotation = FRotator::ZeroRotator;
		auto Location = RootComponent->GetComponentLocation();

		if (!YawClutch)
			Rotation.Yaw = RootComponent->GetComponentRotation().Yaw - 90;
		Location.Z = RootComponent->GetComponentLocation().Z - 34;

		GetMesh()->SetWorldRotation(Rotation);
		GetMesh()->SetWorldLocation(Location);
	}
}
