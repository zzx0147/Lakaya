// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Task/AbilityTask_WaitGameplayTagQueryWrapper.h"

#include "AbilitySystemGlobals.h"

UAbilityTask_WaitGameplayTagQueryWrapper* UAbilityTask_WaitGameplayTagQueryWrapper::WaitGameplayTagQuery(
	UGameplayAbility* OwningAbility, const FGameplayTagQuery TagQuery, const AActor* InOptionalExternalTarget,
	const EWaitGameplayTagQueryTriggerCondition TriggerCondition, const bool bOnlyTriggerOnce)
{
	const auto MyObj = NewAbilityTask<UAbilityTask_WaitGameplayTagQueryWrapper>(OwningAbility);
	MyObj->TagQuery = TagQuery;
	MyObj->SetExternalTargetWrapper(InOptionalExternalTarget);
	MyObj->TriggerCondition = TriggerCondition;
	MyObj->bOnlyTriggerOnce = bOnlyTriggerOnce;

	return MyObj;
}

void UAbilityTask_WaitGameplayTagQueryWrapper::SetExternalTargetWrapper(const AActor* Actor)
{
	if (Actor != nullptr)
	{
		bUseExternalTarget = true;
		OptionalExternalTarget = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	}
}
