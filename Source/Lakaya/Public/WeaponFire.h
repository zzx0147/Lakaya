// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Lockstep.h"
#include "WeaponFire.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponFire : public ULockstep
{
	GENERATED_BODY()
};

/**
 * 
 */
class LAKAYA_API IWeaponFire : public ILockstep
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(Server, Reliable)
	virtual void FireStart(const float& Time);

	UFUNCTION(NetMulticast, Unreliable)
	virtual void FireStartConfirmed(const float& Time);

	UFUNCTION(Server, Reliable)
	virtual void FireStop(const float& Time);

	UFUNCTION(NetMulticast, Unreliable)
	virtual void FireStopConfirmed(const float& Time);

	UFUNCTION(Server, Reliable)
	virtual void SwitchFireMode(const float& Time);
	
	UFUNCTION(NetMulticast, Unreliable)
	virtual void SwitchFireModeConfirmed(const float& Time);
};
