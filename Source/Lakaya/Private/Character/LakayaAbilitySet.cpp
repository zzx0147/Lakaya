// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LakayaAbilitySet.h"


void FLakayaAbilityHandleContainer::ClearAbilities()
{
	if (AbilitySystem.IsValid())
	{
		for (const auto& Handle : Handles)
		{
			AbilitySystem->ClearAbility(Handle);
		}
	}
	Handles.Empty();
	AbilitySystem.Reset();
}

void ULakayaAbilitySet::GiveAbilities(UAbilitySystemComponent* InAbilitySystem,
                                      FLakayaAbilityHandleContainer& OutHandle)
{
	if (!ensure(InAbilitySystem)) return;
	OutHandle.Handles.Empty();
	for (const auto& Ability : Abilities)
	{
		OutHandle.Handles.Emplace(Ability.GiveAbility(InAbilitySystem));
	}
	OutHandle.AbilitySystem = InAbilitySystem;
}
