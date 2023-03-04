// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponAbility.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UWeaponAbility : public UWeaponBase
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable)
	void AbilityStart(const float& Time);

	UFUNCTION(Server, Reliable)
	void AbilityStop(const float& Time);

protected:
	UFUNCTION(NetMulticast, Reliable)
	void AbilityStartNotify(const float& Time);

	UFUNCTION(NetMulticast, Reliable)
	void AbilityStopNotify(const float& Time);

	virtual void AbilityStart_Implementation(const float& Time) { return; }
	virtual void AbilityStop_Implementation(const float& Time) { return; }
	virtual void AbilityStartNotify_Implementation(const float& Time) { return; }
	virtual void AbilityStopNotify_Implementation(const float& Time) { return; }
};
