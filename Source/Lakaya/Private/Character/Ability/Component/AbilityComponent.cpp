// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Component/AbilityComponent.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"

UAbilityComponent::UAbilityComponent(): bActivateOnRemoteServer(0), bActivateOnAI(0)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
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
	}
	Activate();
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
	const bool bTreatTagRequirements = AbilitySystemComponent
		&& AbilitySystemComponent->HasAllMatchingGameplayTags(EnableRequireTags.RequireTags)
		&& !AbilitySystemComponent->HasAnyMatchingGameplayTags(EnableRequireTags.IgnoreTags);

	bool bTreatRemoteServerCond = false;
	bool bTreatAICond = false;
	//TODO: 오너가 폰이 아닌 경우에 폰을 찾도록 하는 로직이 필요할 수 있습니다.
	if (const auto Pawn = GetOwner<APawn>())
	{
		bTreatAICond = Pawn->IsPlayerControlled() ? true : bActivateOnAI;
		bTreatRemoteServerCond = Pawn->IsLocallyControlled() ? true : Pawn->HasAuthority() && bActivateOnRemoteServer;
	}
	return Super::ShouldActivate() && bTreatTagRequirements && bTreatRemoteServerCond && bTreatAICond;
}
