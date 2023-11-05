// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Task/AbilityTask_WaitCustomImmunity.h"

#include "Character/Ability/Component/LakayaAbilitySystemComponent.h"
#include "Character/Ability/CustomImmunity/LakayaAbilityCustomImmunity.h"

UAbilityTask_WaitCustomImmunity* UAbilityTask_WaitCustomImmunity::WaitCustomImmunity(UGameplayAbility* OwningAbility,
	TSubclassOf<ULakayaAbilityCustomImmunity> CustomImmunityClass, bool bTriggerOnce, bool bTriggerWhenSameClass)
{
	const auto MyObj = NewAbilityTask<ThisClass>(OwningAbility);
	MyObj->CustomImmunity = CustomImmunityClass.GetDefaultObject();
	MyObj->bIsTriggerOnce = bTriggerOnce;
	MyObj->bOnlySameClass = bTriggerWhenSameClass;
	return MyObj;
}

void UAbilityTask_WaitCustomImmunity::Activate()
{
	Super::Activate();

	const auto ASC = CastChecked<ULakayaAbilitySystemComponent>(AbilitySystemComponent);
	if (CustomImmunity.IsValid())
	{
		ASC->GiveCustomImmunity(CustomImmunity.Get());
		OnImmunedByCustomImmunityHandle = ASC->OnImmunedByCustomImmunity.AddUObject(
			this, &UAbilityTask_WaitCustomImmunity::OnImmunedByCustomImmunity);
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCanceled.Broadcast();
		}
		EndTask();
	}
}

void UAbilityTask_WaitCustomImmunity::OnDestroy(bool bInOwnerFinished)
{
	if (const auto ASC = Cast<ULakayaAbilitySystemComponent>(AbilitySystemComponent))
	{
		ASC->RemoveCustomImmunity(CustomImmunity.Get());
		ASC->OnImmunedByCustomImmunity.Remove(OnImmunedByCustomImmunityHandle);
	}
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitCustomImmunity::OnImmunedByCustomImmunity(const FGameplayEffectSpec& Spec,
                                                                const ULakayaAbilityCustomImmunity* InCustomImmunity)
{
	if (bOnlySameClass && InCustomImmunity != CustomImmunity.Get())
	{
		return;
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnImmune.Broadcast(Spec, InCustomImmunity);
	}
	if (bIsTriggerOnce)
	{
		EndTask();
	}
}
