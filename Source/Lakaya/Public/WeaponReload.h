// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponReload.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UWeaponReload : public UWeaponBase
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable)
	void ReloadStart(const float& Time);

	UFUNCTION(Server, Reliable)
	void ReloadStop(const float& Time);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void ReloadStartNotify(const float& Time);

	UFUNCTION(NetMulticast, Reliable)
	void ReloadStopNotify(const float& Time);

	virtual void ReloadStart_Implementation(const float& Time) { return; }
	virtual void ReloadStop_Implementation(const float& Time) { return; }
	virtual void ReloadStartNotify_Implementation(const float& Time) { return; }
	virtual void ReloadStopNotify_Implementation(const float& Time) { return; }
};
