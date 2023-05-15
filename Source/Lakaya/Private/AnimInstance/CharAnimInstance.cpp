// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/CharAnimInstance.h"
#include "Character/InteractableCharacter.h"
#include "Character/Ability/LockstepFireAbility.h"
#include "Character/Ability/ResultNotifyFireAbility.h"
#include "Occupation/OccupationObject.h"

void UCharAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	if(const auto Character = Cast<AArmedCharacter>(TryGetPawnOwner()))
	{
		auto& Abilities = Character->GetAbilities();
		// if(Abilities.IsValidIndex(WeaponFire))
		// {
		// 	if(const auto FireAbility = Cast<UAutoFireAbility>(Abilities[WeaponFire]))
		// 	{
		// 		FireAbility->OnFiringStateChanged.
		// 		AddLambda([this](const bool& FireState)
		// 			{bIsAutoFire = FireState;} );
		// 	}
		// }

		if(Abilities.IsValidIndex(WeaponFire))
		{
			if(const auto ResultNotifyFireAbility = Cast<UResultNotifyFireAbility>(Abilities[WeaponFire]))
			{
				ResultNotifyFireAbility->WeaponFireCheck.
				AddLambda([this](const bool& FireState)
					{bIsAutoFire = FireState;} );
				UE_LOG(LogTemp, Error, TEXT("Fire!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
			}
		}

		if(Abilities.IsValidIndex(WeaponReload))
		{
			if(const auto ReloadAbility = Cast<UReloadAbility>(Abilities[WeaponReload]))
			{
				ReloadAbility->OnReloadStateChanged.
				AddLambda([this](const bool& ReloadState)
					{bIsReload = ReloadState;} );
			}
		}

		if (const auto InteractableCharacter = Cast<AInteractableCharacter>(TryGetPawnOwner()))
		{
			InteractableCharacter->OnInteractingActorChanged.
			AddUObject(this, &UCharAnimInstance::OnInteractingActorChanged);
		}
	}
}

void UCharAnimInstance::OnInteractingActorChanged(AActor* NewInteractingActor)
{
	if (const auto InteractableCharacter =
		Cast<AInteractableCharacter>(TryGetPawnOwner()))
	{
		if (NewInteractingActor)
		{
			bIsInteracting = true;
		}
		else
		{
			bIsInteracting = false;
		}
	}
}

// void UCharAnimInstance::OnInteractingActorChanged(AActor* NewInteractingActor)
// {
// 	if (const auto InteractableCharacter =
// 		Cast<AInteractableCharacter>(TryGetPawnOwner()))
// 	{
// 		const EInteractionState InteractionState =
// 			InteractableCharacter->GetInteractionState();
// 		switch (InteractionState)
// 		{
// 		case EInteractionState::OnGoing:
// 			bIsInteracting = true;
// 			break;
// 		case EInteractionState::Success:
// 			bIsInteracting = false;
// 			break;
// 		case EInteractionState::Stopped:
// 			bIsInteracting = false;
// 			break;
// 		case EInteractionState::Canceled:
// 			bIsInteracting = false;
// 			break;
// 		default:
// 			bIsInteracting = false;
// 			break;
// 		}
// 	}
// }