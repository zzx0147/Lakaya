// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharAnimInstance.h"

void UCharAnimInstance::SetIsCrouching(bool CrouchValue)
{
	bIsCrouch = CrouchValue;
}

void UCharAnimInstance::SetState(EFocusState state)
{
	switch (state)
	{
	case EFocusState::None:
		bIsGPSFire = false;
		bIsSwitching = false;
		bIsReload = false;
		bIsInteracting = false;
		break;
	case EFocusState::Firing:
		bIsGPSFire = true;
		break;
	case EFocusState::Switching:
		bIsSwitching = true;
		break;
	case EFocusState::Reloading:
		bIsReload = true;
		break;
	case EFocusState::Interacting:
		bIsInteracting = true;
		break;
	default:;
	}
}

void UCharAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Cast<AFocusableCharacter>(TryGetPawnOwner())->GetFocusChangedEvent
	(EFocusContext::Simulated,EFocusSpace::MainHand).AddUObject(this,&UCharAnimInstance::SetState);
}