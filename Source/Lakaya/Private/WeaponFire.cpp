// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponFire.h"

void UWeaponFire::ExecuteEvent(const uint8& EventNumber)
{
	Super::ExecuteEvent(EventNumber);
	switch (EventNumber)
	{
	case RequestFireStartEvent:
		OnFireStart();
		break;
	case RequestFireStopEvent:
		OnFireStop();
		break;
	case RequestSwitchSelectorEvent:
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

void UWeaponFire::FireStart()
{
	RequestFireStart(GetServerTime());
}

void UWeaponFire::FireStop()
{
	RequestFireStop(GetServerTime());
}

void UWeaponFire::SwitchSelector()
{
	RequestSwitchSelector(GetServerTime());
}

void UWeaponFire::RequestFireStart_Implementation(const float& Time)
{
	FireStartNotify(Time);
	ApplyEvent(RequestFireStartEvent, Time);
}

void UWeaponFire::RequestFireStop_Implementation(const float& Time)
{
	FireStopNotify(Time);
	ApplyEvent(RequestFireStopEvent, Time);
}

void UWeaponFire::RequestSwitchSelector_Implementation(const float& Time)
{
	SwitchSelectorNotify(Time);
	ApplyEvent(RequestSwitchSelectorEvent, Time);
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
