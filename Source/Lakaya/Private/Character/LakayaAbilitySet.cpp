// Fill out your copyright notice in the Description page of Project Settings.


#include "LakayaAbilitySet.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"

void ULakayaAbilitySet::GiveAbilities(UAbilitySystemComponent* AbilitySystemComponent, const int32& InputOffset)
{
	// 어빌리티 시스템 컴포넌트는 유효해야 합니다.
	if (!ensure(AbilitySystemComponent)) return;

	// 이미 어빌리티가 등록되어있었을 수 있으므로, 먼저 어빌리티를 해제합니다.
	ClearAbilities();

	for (const auto& [Input, Class] : Abilities)
	{
		const auto& AbilitySpec = FGameplayAbilitySpec(Class, 1, static_cast<int32>(Input) + InputOffset);
		RegisteredAbilities.Emplace(AbilitySystemComponent->GiveAbility(AbilitySpec));
	}
	RegisteredAbilitySystem = AbilitySystemComponent;
}

void ULakayaAbilitySet::ClearAbilities()
{
	if (!RegisteredAbilitySystem.IsValid()) return;
	for (auto&& Handle : RegisteredAbilities)
	{
		RegisteredAbilitySystem->ClearAbility(Handle);
	}
	RegisteredAbilities.Empty();
	RegisteredAbilitySystem.Reset();
}
