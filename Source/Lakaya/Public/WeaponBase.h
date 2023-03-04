// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "WeaponBase.generated.h"

/**
 * 
 */
UCLASS(Config=Game)
class LAKAYA_API UWeaponBase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void SetupData(const FName& RowName);

protected:
	virtual void SetupData_Implementation(const FName& RowName) { return; }
	
	UPROPERTY(Config)
	float LockstepDelay = 0.1f;
};
