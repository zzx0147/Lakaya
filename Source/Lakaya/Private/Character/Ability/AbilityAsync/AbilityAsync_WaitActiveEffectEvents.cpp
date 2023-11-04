// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/AbilityAsync/AbilityAsync_WaitActiveEffectEvents.h"

#include "AbilitySystemComponent.h"

#define EVENT_SET_VALID_SCOPE()\
	const auto ASC = GetAbilitySystemComponent();\
	if (!ASC)\
	{\
		if (ShouldBroadcastDelegates())\
		{\
			OnCanceled.Broadcast();\
		}\
		EndAction();\
		return;\
	}\
	const auto EventSet = ASC->GetActiveEffectEventSet(GetActiveHandle());\
	if (!EventSet)\
	{\
		if (ShouldBroadcastDelegates())\
		{\
			OnCanceled.Broadcast();\
		}\
		EndAction();\
		return;\
	}

#define EVENT_SET_OPTIONAL_ACTION(OnValid)\
	if (const auto ASC = GetAbilitySystemComponent())\
	{\
		if (const auto EventSet = ASC->GetActiveEffectEventSet(GetActiveHandle()))\
		{\
			EventSet->OnValid;\
		}\
	}

UAbilityAsync_WaitActiveEffectRemoved* UAbilityAsync_WaitActiveEffectRemoved::WaitActiveEffectRemoved(
	FActiveGameplayEffectHandle InActiveHandle)
{
	return NewActiveEffectAsync<ThisClass>(InActiveHandle);
}

void UAbilityAsync_WaitActiveEffectRemoved::Activate()
{
	EVENT_SET_VALID_SCOPE()
	OnActiveEffectRemovedDelegateHandle = EventSet->OnEffectRemoved.AddUObject(this, &ThisClass::OnActiveEffectRemoved);
}

void UAbilityAsync_WaitActiveEffectRemoved::EndAction()
{
	if (OnActiveEffectRemovedDelegateHandle.IsValid())
	{
		EVENT_SET_OPTIONAL_ACTION(OnEffectRemoved.Remove(OnActiveEffectRemovedDelegateHandle))
	}
	Super::EndAction();
}

void UAbilityAsync_WaitActiveEffectRemoved::OnActiveEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo)
{
	if (ShouldBroadcastDelegates())
	{
		OnRemoved.Broadcast(RemovalInfo);
	}
	EndAction();
}

UAbilityAsync_WaitActiveEffectStackCountChanged* UAbilityAsync_WaitActiveEffectStackCountChanged::
WaitActiveEffectStackCountChanged(FActiveGameplayEffectHandle InActiveHandle, bool bTriggerOnce)
{
	const auto MyObj = NewActiveEffectAsync<ThisClass>(InActiveHandle);
	MyObj->bIsTriggerOnce = bTriggerOnce;
	return MyObj;
}

void UAbilityAsync_WaitActiveEffectStackCountChanged::Activate()
{
	EVENT_SET_VALID_SCOPE()
	OnActiveEffectStackChangedDelegateHandle = EventSet->OnStackChanged.AddUObject(
		this, &ThisClass::OnActiveEffectStackChanged);
}

void UAbilityAsync_WaitActiveEffectStackCountChanged::EndAction()
{
	if (OnActiveEffectStackChangedDelegateHandle.IsValid())
	{
		EVENT_SET_OPTIONAL_ACTION(OnStackChanged.Remove(OnActiveEffectStackChangedDelegateHandle))
	}
	Super::EndAction();
}

void UAbilityAsync_WaitActiveEffectStackCountChanged::OnActiveEffectStackChanged(FActiveGameplayEffectHandle Handle,
	int32 NewStackCount, int32 OldStackCount)
{
	if (ShouldBroadcastDelegates())
	{
		OnStackChanged.Broadcast(NewStackCount, OldStackCount);
	}
	if (bIsTriggerOnce)
	{
		EndAction();
	}
}

UAbilityAsync_WaitActiveEffectTimeChanged* UAbilityAsync_WaitActiveEffectTimeChanged::WaitActiveEffectTimeChanged(
	FActiveGameplayEffectHandle InActiveHandle, bool bTriggerOnce)
{
	const auto MyObj = NewActiveEffectAsync<ThisClass>(InActiveHandle);
	MyObj->bIsTriggerOnce = bTriggerOnce;
	return MyObj;
}

void UAbilityAsync_WaitActiveEffectTimeChanged::Activate()
{
	EVENT_SET_VALID_SCOPE()
	OnActiveEffectTimeChangedDelegateHandle = EventSet->OnTimeChanged.AddUObject(
		this, &ThisClass::OnActiveEffectTimeChanged);
}

void UAbilityAsync_WaitActiveEffectTimeChanged::EndAction()
{
	if (OnActiveEffectTimeChangedDelegateHandle.IsValid())
	{
		EVENT_SET_OPTIONAL_ACTION(OnTimeChanged.Remove(OnActiveEffectTimeChangedDelegateHandle))
	}
	Super::EndAction();
}

void UAbilityAsync_WaitActiveEffectTimeChanged::OnActiveEffectTimeChanged(FActiveGameplayEffectHandle Handle,
                                                                          float NewStartTime, float NewDuration)
{
	if (ShouldBroadcastDelegates())
	{
		OnTimeChanged.Broadcast(NewStartTime, NewDuration);
	}
	if (bIsTriggerOnce)
	{
		EndAction();
	}
}

UAbilityAsync_WaitActiveEffectInhibitionChanged* UAbilityAsync_WaitActiveEffectInhibitionChanged::
WaitActiveEffectInhibitionChanged(FActiveGameplayEffectHandle InActiveHandle, bool bTriggerOnce)
{
	const auto MyObj = NewActiveEffectAsync<ThisClass>(InActiveHandle);
	MyObj->bIsTriggerOnce = bTriggerOnce;
	return MyObj;
}

void UAbilityAsync_WaitActiveEffectInhibitionChanged::Activate()
{
	EVENT_SET_VALID_SCOPE()
	OnActiveEffectInhibitionChangedDelegateHandle = EventSet->OnInhibitionChanged.AddUObject(
		this, &ThisClass::OnActiveEffectInhibitionChanged);
}

void UAbilityAsync_WaitActiveEffectInhibitionChanged::EndAction()
{
	if (OnActiveEffectInhibitionChangedDelegateHandle.IsValid())
	{
		EVENT_SET_OPTIONAL_ACTION(OnInhibitionChanged.Remove(OnActiveEffectInhibitionChangedDelegateHandle))
	}
	Super::EndAction();
}

void UAbilityAsync_WaitActiveEffectInhibitionChanged::OnActiveEffectInhibitionChanged(
	FActiveGameplayEffectHandle Handle, bool bIsInhibited)
{
	if (ShouldBroadcastDelegates())
	{
		OnInhibitionChanged.Broadcast(bIsInhibited);
	}
	if (bIsTriggerOnce)
	{
		EndAction();
	}
}
