// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Lockstep.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public ULockstep
{
	GENERATED_BODY()
};

/**
 * 
 */
class LAKAYA_API IInteractable : public ILockstep
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(Server, Reliable)
	virtual void InteractionStart(const float& Time, APawn* Caller);
	
	UFUNCTION(Server, Reliable)
	virtual void InteractionStop(const float& Time, APawn* Caller);
};
