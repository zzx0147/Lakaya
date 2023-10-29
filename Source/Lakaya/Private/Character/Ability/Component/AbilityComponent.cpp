// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Component/AbilityComponent.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

UAbilityComponent::UAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAbilityComponent::SetOwningAbility(UGameplayAbility* InOwningAbility)
{
	if (!ensure(IsValid(InOwningAbility)))
	{
		return;
	}

	OwningAbility = InOwningAbility;
	AbilitySystemComponent = OwningAbility->GetAbilitySystemComponentFromActorInfo_Checked();

	if (!EnableRequireTags.IsEmpty())
	{
		for (auto&& Tag : EnableRequireTags.RequireTags)
		{
			AbilitySystemComponent->RegisterGameplayTagEvent(Tag).AddUObject(this, &ThisClass::OnRequiredTagUpdated);
		}
		for (auto&& Tag : EnableRequireTags.IgnoreTags)
		{
			AbilitySystemComponent->RegisterGameplayTagEvent(Tag).AddUObject(this, &ThisClass::OnIgnoredTagUpdated);
		}
		ToggleActive();
	}
	else
	{
		Activate();
	}
}

void UAbilityComponent::OnRequiredTagUpdated(FGameplayTag Tag, int32 NewCount)
{
	if (ensure(AbilitySystemComponent))
	{
		SetActive(NewCount > 0);
	}
}

void UAbilityComponent::OnIgnoredTagUpdated(FGameplayTag Tag, int32 NewCount)
{
	if (ensure(AbilitySystemComponent))
	{
		SetActive(NewCount == 0);
	}
}

bool UAbilityComponent::ShouldActivate() const
{
	return Super::ShouldActivate() && AbilitySystemComponent
		&& AbilitySystemComponent->HasAllMatchingGameplayTags(EnableRequireTags.RequireTags)
		&& !AbilitySystemComponent->HasAnyMatchingGameplayTags(EnableRequireTags.IgnoreTags);
}
