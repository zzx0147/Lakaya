// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovableCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

AMovableCharacter::AMovableCharacter()
{
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}
