// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponReload.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class LAKAYA_API UWeaponReload : public UWeaponBase
{
	GENERATED_BODY()

protected:
	enum EWeaponReloadEvent : uint8
	{
		ReloadStartEvent,
		ReloadStopEvent,
		ReloadStartNotifyEvent,
		ReloadStopNotifyEvent
	};

	virtual void ExecuteEvent(const uint8& EventNumber) override;

public:
	virtual void ReloadStart();
	virtual void ReloadStop();

protected:
	UFUNCTION(Server, Reliable)
	void RequestReloadStart(const float& Time);

	UFUNCTION(Server, Reliable)
	void RequestReloadStop(const float& Time);

	UFUNCTION(NetMulticast, Reliable)
	void ReloadStartNotify(const float& Time);

	UFUNCTION(NetMulticast, Reliable)
	void ReloadStopNotify(const float& Time);

	virtual void RequestReloadStart_Implementation(const float& Time);
	virtual void RequestReloadStop_Implementation(const float& Time);
	virtual void ReloadStartNotify_Implementation(const float& Time);
	virtual void ReloadStopNotify_Implementation(const float& Time);

	virtual void OnReloadStart() { return; }
	virtual void OnReloadStop() { return; }
	virtual void OnReloadStartNotify() { return; }
	virtual void OnReloadStopNotify() { return; }
};
