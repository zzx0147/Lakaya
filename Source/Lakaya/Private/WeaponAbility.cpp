// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAbility.h"

void UWeaponAbility::ExecuteEvent(const uint8& EventNumber)
{
	Super::ExecuteEvent(EventNumber);

	switch (EventNumber)
	{
	case RequestAbilityStartEvent:
		OnAbilityStart();
		break;
	case RequestAbilityStopEvent:
		OnAbilityStop();
		break;
	case AbilityStartNotifyEvent:
		OnAbilityStartNotify();
		break;
	case AbilityStopNotifyEvent:
		OnAbilityStopNotify();
		break;
	default:
		UE_LOG(LogNetSubObject, Warning, TEXT("ExecuteEvent called not initialized eventnumber. It was %d"),
		       EventNumber);
		break;
	}
}

void UWeaponAbility::RequestAbilityStart_Implementation(const float& Time)
{
	if (!GetIsEnabled()) return;
	AbilityStartNotify(Time);
	ApplyEvent(RequestAbilityStartEvent, Time);
}

void UWeaponAbility::RequestAbilityStop_Implementation(const float& Time)
{
	if (!GetIsEnabled()) return;
	AbilityStopNotify(Time);
	ApplyEvent(RequestAbilityStopEvent, Time);
}

void UWeaponAbility::AbilityStartNotify_Implementation(const float& Time)
{
	if (!GetIsEnabled()) return;
	ApplyEvent(AbilityStartNotifyEvent, Time);
}

void UWeaponAbility::AbilityStopNotify_Implementation(const float& Time)
{
	if (!GetIsEnabled()) return;
	ApplyEvent(AbilityStopNotifyEvent, Time);
}
