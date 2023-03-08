// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponFire.h"

void UWeaponFire::ExecuteEvent(const uint8& EventNumber)
{
	Super::ExecuteEvent(EventNumber);
	switch (EventNumber)
	{
	case FireStartEvent:
		OnFireStart();
		break;
	case FireStopEvent:
		OnFireStop();
		break;
	case SwitchSelectorEvent:
		OnSwitchSelector();
		break;
	case FireStartNotifyEvent:
		OnFireStartNotify();
		break;
	case FireStopNotifyEvent:
		OnFireStopNotify();
		break;
	case SwitchSelectorNotifyEvent:
		OnSwitchSelectorNotify();
		break;
	default:
		UE_LOG(LogNetSubObject, Warning, TEXT("ExecuteEvent called not initialized eventnumber. It was %d"),
		       EventNumber);
		break;
	}
}

void UWeaponFire::FireStart_Implementation(const float& Time)
{
	FireStartNotify(Time);
	ApplyEvent(FireStartEvent, Time);
}

void UWeaponFire::FireStop_Implementation(const float& Time)
{
	FireStopNotify(Time);
	ApplyEvent(FireStopEvent, Time);
}

void UWeaponFire::SwitchSelector_Implementation(const float& Time)
{
	SwitchSelectorNotify(Time);
	ApplyEvent(SwitchSelectorEvent, Time);
}

void UWeaponFire::FireStartNotify_Implementation(const float& Time)
{
	ApplyEvent(FireStartNotifyEvent, Time);
}

void UWeaponFire::FireStopNotify_Implementation(const float& Time)
{
	ApplyEvent(FireStopNotifyEvent, Time);
}

void UWeaponFire::SwitchSelectorNotify_Implementation(const float& Time)
{
	ApplyEvent(SwitchSelectorNotifyEvent, Time);
}
