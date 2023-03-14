// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAbility.h"
#include "ZoomAbility.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UZoomAbility : public UWeaponAbility
{
	GENERATED_BODY()

public:
	virtual void AbilityStart() override;
	virtual void AbilityStop() override;

protected:
	virtual void SetupData(const FName& RowName) override;

private:
	UPROPERTY(Replicated)
	float FOVMultiplier;

	UPROPERTY(Replicated)
	TWeakObjectPtr<class UCameraComponent> Camera;
};
