// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Task/AbilityTask_InhibitInput.h"

#include "Character/Ability/Component/LakayaAbilitySystemComponent.h"

UAbilityTask_InhibitInput* UAbilityTask_InhibitInput::InhibitInput(UGameplayAbility* OwningAbility,
                                                                   const TSet<int32>& InputIDSet)
{
	const auto MyObj = NewAbilityTask<UAbilityTask_InhibitInput>(OwningAbility);
	MyObj->InhibitedInputIDSet = InputIDSet;
	return MyObj;
}

void UAbilityTask_InhibitInput::Activate()
{
	Super::Activate();
	const auto ASC = CastChecked<ULakayaAbilitySystemComponent>(AbilitySystemComponent);
	for (const auto& InputID : InhibitedInputIDSet)
	{
		ASC->AddInputInhibitedDelegate(
			InputID, FInputInhibitedDelegate::CreateUObject(this, &ThisClass::OnInputPressInhibition),
			FInputInhibitedDelegate::CreateUObject(this, &ThisClass::OnInputReleaseInhibition));
	}
}

void UAbilityTask_InhibitInput::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
	if (const auto ASC = Cast<ULakayaAbilitySystemComponent>(AbilitySystemComponent))
	{
		for (const auto& InputID : InhibitedInputIDSet)
		{
			ASC->RemoveInputInhibitedDelegate(InputID);
		}
	}
}

void UAbilityTask_InhibitInput::OnInputPressInhibition(const int32& InputID)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnInputPressed.Broadcast(InputID);
	}
}

void UAbilityTask_InhibitInput::OnInputReleaseInhibition(const int32& InputID)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnInputReleased.Broadcast(InputID);
	}
}
