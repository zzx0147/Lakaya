// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponReload.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponReload : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LAKAYA_API IWeaponReload
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(Server, Reliable)
	virtual void ReloadStart();

	UFUNCTION(Server, Reliable)
	virtual void ReloadStop();
};
