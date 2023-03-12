// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponFire.h"
#include "RiffleFireCore.generated.h"

UENUM(BlueprintType)
enum class EGunSelector : uint8
{
	Semi,
	Burst,
	Auto
};

/**
 * 
 */
UCLASS(Abstract)
class LAKAYA_API URiffleFireCore : public UWeaponFire
{
	GENERATED_BODY()

protected:
	void FireStartCore(FTimerHandle& GunSelectorTimer, FTimerHandle& GunFireTimer,
	                   std::function<bool()> EmptyDeterminant, std::function<void()> OnEmptyMag,
	                   std::function<void()> NestedFire, std::function<void()> FreshFire);

	void FireStopCore(const EGunSelector& GunSelector, uint16& GunFireCount);

	void SwitchSelectorCore(const uint16& GunFireCount, EGunSelector& GunSelector, FTimerHandle& GunSelectorTimer,
	                        std::function<void()> OnUpdateSelector);

	void NestedFireCore(const EGunSelector& GunSelector, uint16& GunFireCount);

	void FreshFireCore(const EGunSelector& GunSelector, uint16& GunFireCount, FTimerHandle& FireTimerHandle,
	                   std::function<void()> FireFunction);

	void FireCallback(uint16& GunFireCount, FTimerHandle& FireTimerHandle, std::function<bool()> EmptyDeterminant,
	                  std::function<void()> OnEmpty, std::function<void()> FireFunction);

	void StopFireCore(uint16& GunFireCount, FTimerHandle& FireTimerHandle);

	UPROPERTY(Replicated)
	TWeakObjectPtr<class UGunComponent> GunComponent;

	UPROPERTY(Replicated)
	TWeakObjectPtr<class AThirdPersonCharacter> Character;

	UPROPERTY(Replicated)
	float BaseDamage;

	UPROPERTY(Replicated)
	float FireDelay;

	UPROPERTY(Replicated)
	float FireRange;

	UPROPERTY(Replicated)
	float SqrFireRange;

	UPROPERTY(Replicated)
	float SwitchingDelay;
};
