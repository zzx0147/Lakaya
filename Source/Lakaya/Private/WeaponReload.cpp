// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponReload.h"

void UWeaponReload::ExecuteEvent(const uint8& EventNumber)
{
	Super::ExecuteEvent(EventNumber);

	switch (EventNumber)
	{
	case ReloadStartEvent:
		OnReloadStart();
		break;
	case ReloadStopEvent:
		OnReloadStop();
		break;
	case ReloadStartNotifyEvent:
		OnReloadStartNotify();
		break;
	case ReloadStopNotifyEvent:
		OnReloadStopNotify();
		break;
	default:
		UE_LOG(LogNetSubObject, Warning, TEXT("ExecuteEvent called not initialized eventnumber. It was %d"),
		       EventNumber);
		break;
	}
}

void UWeaponReload::ReloadStart_Implementation(const float& Time)
{
	if (!GetIsEnabled()) return;
	ReloadStartNotify(Time);
	ApplyEvent(ReloadStartEvent, Time);
}

void UWeaponReload::ReloadStop_Implementation(const float& Time)
{
	if (!GetIsEnabled()) return;
	ReloadStopNotify(Time);
	ApplyEvent(ReloadStopEvent, Time);
}

void UWeaponReload::ReloadStartNotify_Implementation(const float& Time)
{
	if (!GetIsEnabled()) return;
	ApplyEvent(ReloadStartNotifyEvent, Time);
}

void UWeaponReload::ReloadStopNotify_Implementation(const float& Time)
{
	if (!GetIsEnabled()) return;
	ApplyEvent(ReloadStopNotifyEvent, Time);
}
