// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponFire.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class LAKAYA_API UWeaponFire : public UWeaponBase
{
	GENERATED_BODY()

protected:
	enum EWeaponFireEvent : uint8
	{
		RequestFireStartEvent,
		RequestFireStopEvent,
		RequestSwitchSelectorEvent,
		FireStartNotifyEvent,
		FireStopNotifyEvent,
		SwitchSelectorNotifyEvent
	};

	virtual void ExecuteEvent(const uint8& EventNumber) override;

public:
	virtual void FireStart();
	virtual void FireStop();
	virtual void SwitchSelector();

protected:
	UFUNCTION(Server, Reliable)
	void RequestFireStart(const float& Time);

	UFUNCTION(Server, Reliable)
	void RequestFireStop(const float& Time);

	UFUNCTION(Server, Reliable)
	void RequestSwitchSelector(const float& Time);

	UFUNCTION(NetMulticast, Reliable)
	void FireStartNotify(const float& Time);

	UFUNCTION(NetMulticast, Reliable)
	void FireStopNotify(const float& Time);

	UFUNCTION(NetMulticast, Reliable)
	void SwitchSelectorNotify(const float& Time);

	virtual void RequestFireStart_Implementation(const float& Time);
	virtual void RequestFireStop_Implementation(const float& Time);
	virtual void RequestSwitchSelector_Implementation(const float& Time);
	virtual void FireStartNotify_Implementation(const float& Time);
	virtual void FireStopNotify_Implementation(const float& Time);
	virtual void SwitchSelectorNotify_Implementation(const float& Time);

	virtual void OnFireStart() { return; }
	virtual void OnFireStop() { return; }
	virtual void OnSwitchSelector() { return; }
	virtual void OnFireStartNotify() { return; }
	virtual void OnFireStopNotify() { return; }
	virtual void OnSwitchSelectorNotify() { return; }
};
