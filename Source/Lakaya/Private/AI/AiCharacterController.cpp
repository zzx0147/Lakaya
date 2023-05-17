// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AiCharacterController.h"

#include "Character/ArmedCharacter.h"

AAiCharacterController::AAiCharacterController() // 생성자
{
	bWantsPlayerState = true;
}

void AAiCharacterController::AIFireStart(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StartAbility(WeaponFire);
}

void AAiCharacterController::AIFireStop(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StopAbility(WeaponFire);
}
