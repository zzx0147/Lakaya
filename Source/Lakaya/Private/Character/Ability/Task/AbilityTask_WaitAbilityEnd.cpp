// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Task/AbilityTask_WaitAbilityEnd.h"

#include "AbilitySystemComponent.h"

UAbilityTask_WaitAbilityEnd* UAbilityTask_WaitAbilityEnd::WaitAbilityEnd(
	UGameplayAbility* InOwningAbility, const FGameplayAbilitySpecHandle& InAbilityHandle, const bool& bInInvokeOnce)
{
	const auto TaskInstance = NewAbilityTask<ThisClass>(InOwningAbility);
	TaskInstance->AbilityHandle = InAbilityHandle;
	TaskInstance->bInvokeOnce = bInInvokeOnce;
	return TaskInstance;
}

void UAbilityTask_WaitAbilityEnd::Activate()
{
	Super::Activate();
	if (ensure(AbilitySystemComponent.IsValid()))
	{
		AbilityEndedDelegateHandle =
			AbilitySystemComponent->OnAbilityEnded.AddUObject(this, &ThisClass::AbilityEnded);
	}
}

void UAbilityTask_WaitAbilityEnd::OnDestroy(bool bInOwnerFinished)
{
	if (AbilityEndedDelegateHandle.IsValid())
	{
		AbilitySystemComponent->OnAbilityEnded.Remove(AbilityEndedDelegateHandle);
	}
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitAbilityEnd::AbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (AbilityEndedData.AbilitySpecHandle != AbilityHandle)
	{
		return;
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnAbilityEnded.Broadcast(AbilityEndedData);
	}

	if (bInvokeOnce)
	{
		EndTask();
	}
}
