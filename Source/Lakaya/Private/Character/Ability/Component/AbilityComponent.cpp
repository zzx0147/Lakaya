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
		const auto Delegate =
			FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &ThisClass::OnGameplayTagEvent);

		for (auto&& Tag : EnableRequireTags.RequireTags)
		{
			AbilitySystemComponent->RegisterAndCallGameplayTagEvent(Tag, Delegate);
		}
		for (auto&& Tag : EnableRequireTags.IgnoreTags)
		{
			AbilitySystemComponent->RegisterAndCallGameplayTagEvent(Tag, Delegate);
		}
	}
}

void UAbilityComponent::OnGameplayTagEvent(FGameplayTag Tag, int32 NewCount)
{
	//TODO: 태그 이벤트가 호출될 때 마다 O(n)짜리 루프를 두 번씩 돌리고 있어서 아주 비효율적입니다. 나중에 수정되어야 합니다.
	ToggleActive();
}

bool UAbilityComponent::ShouldActivate() const
{
	return Super::ShouldActivate() && AbilitySystemComponent
		&& AbilitySystemComponent->HasAllMatchingGameplayTags(EnableRequireTags.RequireTags)
		&& !AbilitySystemComponent->HasAnyMatchingGameplayTags(EnableRequireTags.IgnoreTags);
}
