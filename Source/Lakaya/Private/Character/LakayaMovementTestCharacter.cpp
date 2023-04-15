// Fill out your copyright notice in the Description page of Project Settings.


#include "Movement/LakayaCharacterMovementComponent.h"
#include "Character/LakayaMovementTestCharacter.h"

ALakayaMovementTestCharacter::ALakayaMovementTestCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<ULakayaCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{

	LakayaCharacterMovementComponent = Cast<ULakayaCharacterMovementComponent>(GetCharacterMovement());

}                                     