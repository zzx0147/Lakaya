// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponFire.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponFire : public UWeaponBase
{
	GENERATED_BODY()
};

/**
 * 
 */
class LAKAYA_API IWeaponFire : public IWeaponBase
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(Server, Reliable)
	virtual void FireStart(const float& Time);

	UFUNCTION(Server, Reliable)
	virtual void FireStop(const float& Time);

	UFUNCTION(Server, Reliable)
	virtual void SwitchSelector(const float& Time);

protected:
	UFUNCTION(NetMulticast, Reliable)
	virtual void FireStartNotify(const float& Time);

	UFUNCTION(NetMulticast, Reliable)
	virtual void FireStopNotify(const float& Time);
};
