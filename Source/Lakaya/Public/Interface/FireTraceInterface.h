// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FireTraceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UFireTraceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LAKAYA_API IFireTraceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void FireTrace(TArray<FHitResult>& OutHitResults);
};
