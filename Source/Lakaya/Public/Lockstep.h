// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Lockstep.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULockstep : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LAKAYA_API ILockstep
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	void Execute(void (*ExecuteFunction)(UObject*, const float&));
	void Execute(void (*ExecuteFunction)(UObject*, const float&, APawn*), APawn* Caller);
};
