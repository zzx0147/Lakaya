// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharAnimInstance.h"

void UCharAnimInstance::SetIsGPSFire(bool GPSFireValue)
{
	bIsGPSFire = GPSFireValue;
}

void UCharAnimInstance::SetIsReload(bool ReloadValue)
{
	bIsReload = ReloadValue;
}

void UCharAnimInstance::SetIsCrouching(bool CrouchValue)
{
	bIsCrouch = CrouchValue;
}
