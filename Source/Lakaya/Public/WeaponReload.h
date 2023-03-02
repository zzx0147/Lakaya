// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponReload.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponReload : public UWeaponBase
{
	GENERATED_BODY()
};

/**
 * 
 */
class LAKAYA_API IWeaponReload : public IWeaponBase
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(Server, Reliable)
	virtual void ReloadStart(const float& Time);

	UFUNCTION(Server, Reliable)
	virtual void ReloadStop(const float& Time);

protected:
	UFUNCTION(NetMulticast, Reliable)
	virtual void ReloadStartNotify(const float& Time);

	UFUNCTION(NetMulticast, Reliable)
	virtual void ReloadStopNotify(const float& Time);
};
