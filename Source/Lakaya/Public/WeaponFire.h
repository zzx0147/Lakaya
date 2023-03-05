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

public:
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

	virtual void FireStart_Implementation(const float& Time) { return; }
	virtual void FireStop_Implementation(const float& Time) { return; }
	virtual void SwitchSelector_Implementation(const float& Time) { return; }
	virtual void FireStartNotify_Implementation(const float& Time) { return; }
	virtual void FireStopNotify_Implementation(const float& Time) { return; }
	virtual void SwitchSelectorNotify_Implementation(const float& Time) { return; }
};
