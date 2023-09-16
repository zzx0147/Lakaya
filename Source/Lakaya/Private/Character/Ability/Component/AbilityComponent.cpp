// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Component/AbilityComponent.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

void UAbilityComponent::SetOwningAbility(UGameplayAbility* InOwningAbility)
{
	if (!ensure(IsValid(InOwningAbility)))
	{
		return;
	}

	OwningAbility = InOwningAbility;
	AbilitySystemComponent = OwningAbility->GetAbilitySystemComponentFromActorInfo();

	if (!ensure(AbilitySystemComponent.IsValid()))
	{
		return;
	}

	if (!DisableGameplayTags.IsEmpty())
	{
		const auto Delegate =
			FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &ThisClass::OnGameplayTagEvent);

		for (auto&& Tag : DisableGameplayTags)
		{
			AbilitySystemComponent->RegisterAndCallGameplayTagEvent(Tag, Delegate);
		}
	}
}

void UAbilityComponent::OnGameplayTagEvent(FGameplayTag Tag, int32 NewCount)
{
	if (!ensure(AbilitySystemComponent.IsValid()))
	{
		return;
	}

	if ((NewCount == 0) == bIsDisabled
		&& bIsDisabled != AbilitySystemComponent->HasAnyMatchingGameplayTags(DisableGameplayTags))
	{
		bIsDisabled = !bIsDisabled;
		bIsDisabled ? OnDisabled() : OnEnabled();
	}
}

void UAbilityComponent::OnEnabled_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("%s enabled "), *GetName());
}

void UAbilityComponent::OnDisabled_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("%s disabled"), *GetName());
}
