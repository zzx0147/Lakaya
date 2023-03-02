// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Lockstep.h"
#include "WeaponAbility.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponAbility : public ULockstep
{
	GENERATED_BODY()
};

/**
 * 
 */
class LAKAYA_API IWeaponAbility : public ILockstep
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(Server, Reliable)
	virtual void AbilityStart(const float& Time);

	UFUNCTION(Server, Reliable)
	virtual void AbilityStop(const float& Time);
};
