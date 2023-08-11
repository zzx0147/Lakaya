// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Task/AbilityTask_WaitAbilityCooldown.h"

#include "AbilitySystemComponent.h"
#include "Character/Ability/Task/AbilityTask_WaitGameplayTagQueryWrapper.h"

UAbilityTask_WaitAbilityCooldown* UAbilityTask_WaitAbilityCooldown::WaitAbilityCooldown(
	UGameplayAbility* InOwningAbility,
	const FGameplayAbilitySpecHandle& InAbilityHandle)
{
	const auto TaskInstance = NewAbilityTask<ThisClass>(InOwningAbility);
	TaskInstance->AbilityHandle = InAbilityHandle;
	return TaskInstance;
}

void UAbilityTask_WaitAbilityCooldown::Activate()
{
	Super::Activate();
	if (ensure(AbilitySystemComponent.IsValid()))
	{
		AbilityEndedDelegateHandle = AbilitySystemComponent->OnAbilityEnded.
		                                                     AddUObject(this, &ThisClass::OnAbilityEnded);
	}
}

void UAbilityTask_WaitAbilityCooldown::OnDestroy(bool bInOwnerFinished)
{
	if (AbilityEndedDelegateHandle.IsValid())
	{
		AbilitySystemComponent->OnAbilityEnded.Remove(AbilityEndedDelegateHandle);
	}
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitAbilityCooldown::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (AbilityEndedData.AbilitySpecHandle != AbilityHandle)
	{
		return;
	}

	if (ensure(AbilitySystemComponent.IsValid()))
	{
		const auto CooldownTags = AbilityEndedData.AbilityThatEnded->GetCooldownTags();

		const auto TagTask = UAbilityTask_WaitGameplayTagQueryWrapper::WaitGameplayTagQuery(
			Ability, FGameplayTagQuery::MakeQuery_MatchNoTags(*CooldownTags));
		TagTask->OnTriggered().AddDynamic(this, &ThisClass::InvokeOnCooldownEnded);
		TagTask->ReadyForActivation();
	}
}

void UAbilityTask_WaitAbilityCooldown::InvokeOnCooldownEnded()
{
	OnCooldownEnded.Broadcast();
}
