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
		FireStartEvent,
		FireStopEvent,
		SwitchSelectorEvent,
		FireStartNotifyEvent,
		FireStopNotifyEvent,
		SwitchSelectorNotifyEvent
	};

	virtual void ExecuteEvent(const uint8& EventNumber) override;

public:
	//TODO: 클라이언트에서 처리가능한 부분은 클라이언트에서 최대한 처리를 하고 서버 RPC를 콜하도록 로직을 변경해야 합니다.
	UFUNCTION(Server, Reliable)
	void FireStart(const float& Time);

	UFUNCTION(Server, Reliable)
	void FireStop(const float& Time);

	UFUNCTION(Server, Reliable)
	void SwitchSelector(const float& Time);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void FireStartNotify(const float& Time);

	UFUNCTION(NetMulticast, Reliable)
	void FireStopNotify(const float& Time);

	UFUNCTION(NetMulticast, Reliable)
	void SwitchSelectorNotify(const float& Time);

	virtual void FireStart_Implementation(const float& Time);
	virtual void FireStop_Implementation(const float& Time);
	virtual void SwitchSelector_Implementation(const float& Time);
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
