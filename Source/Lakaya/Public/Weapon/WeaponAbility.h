// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponAbility.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class LAKAYA_API UWeaponAbility : public UWeaponBase
{
	GENERATED_BODY()

protected:
	enum EWeaponAbilityEvent : uint8
	{
		RequestAbilityStartEvent,
		RequestAbilityStopEvent,
		AbilityStartNotifyEvent,
		AbilityStopNotifyEvent
	};

	virtual void ExecuteEvent(const uint8& EventNumber) override;

public:
	virtual void AbilityStart();
	virtual void AbilityStop();

protected:
	UFUNCTION(Server, Reliable)
	void RequestAbilityStart(const float& Time);

	UFUNCTION(Server, Reliable)
	void RequestAbilityStop(const float& Time);

	UFUNCTION(NetMulticast, Reliable)
	void AbilityStartNotify(const float& Time);

	UFUNCTION(NetMulticast, Reliable)
	void AbilityStopNotify(const float& Time);

	virtual void RequestAbilityStart_Implementation(const float& Time);
	virtual void RequestAbilityStop_Implementation(const float& Time);
	virtual void AbilityStartNotify_Implementation(const float& Time);
	virtual void AbilityStopNotify_Implementation(const float& Time);

	virtual void OnAbilityStart() { return; }
	virtual void OnAbilityStop() { return; }
	virtual void OnAbilityStartNotify() { return; }
	virtual void OnAbilityStopNotify() { return; }
};
