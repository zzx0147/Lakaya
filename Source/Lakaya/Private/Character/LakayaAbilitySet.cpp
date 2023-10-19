// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LakayaAbilitySet.h"


void FLakayaAbilityHandleContainer::ClearAbilities()
{
	if (AbilitySystem.IsValid())
	{
		for (const auto& Handle : Handles)
		{
			const auto Ability = AbilitySystem->FindAbilitySpecFromHandle(Handle);
			if (!(ensure(Ability))) continue;

			// ASC의 ClearAbility에 NonInstanced 어빌리티에 대한 EndAbility 호출 처리가 없으므로, 여기서 처리합니다.
			if (Ability->IsActive())
			{
				UE_LOG(LogTemp, Log, TEXT("Found active ability %s. It will be canceled and cleared."),
				       *Ability->Ability->GetName());
				AbilitySystem->CancelAbilityHandle(Handle);
			}
			AbilitySystem->ClearAbility(Handle);
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to clear abilities. AbilitySystem is not valid."))
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
