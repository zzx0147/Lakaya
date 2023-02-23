// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AThirdPersonCharacter::AThirdPersonCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}
