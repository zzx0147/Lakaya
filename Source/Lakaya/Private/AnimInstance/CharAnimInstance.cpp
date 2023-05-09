// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/CharAnimInstance.h"

void UCharAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	if(const auto Character = Cast<AArmedCharacter>(TryGetPawnOwner()))
	{
		auto& Abilities = Character->GetAbilities();
		if(Abilities.IsValidIndex(WeaponFire))
		{
			if(const auto FireAbility = Cast<UAutoFireAbility>(Abilities[WeaponFire]))
			{
				FireAbility->OnFiringStateChanged.
				AddLambda([this](const bool& FireState)
					{bIsAutoFire = FireState;} );
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

		// if(Abilities.IsValidIndex(WeaponAbility))
		// {
		// 	if(const auto WeaponSkillAbility = Cast<UReloadAbility>(Abilities[WeaponAbility]))
		// 	{
		// 		WeaponSkillAbility->OnFiringStateChanged.
		// 		AddLambda([this](const bool& WeaponSkillState)
		// 			{bIsWeaponSkill = WeaponSkillState;} );
		// 	}
		// }
	}

	
	// Cast<AFocusableCharacter>(TryGetPawnOwner())->GetFocusChangedEvent
	// (EFocusContext::Simulated,EFocusSpace::MainHand).AddUObject(this,&UCharAnimInstance::SetState);
}