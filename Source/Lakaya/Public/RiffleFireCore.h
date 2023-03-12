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
	void FireStartCore(FTimerHandle& SelectorTimer, FTimerHandle& FireTimer,
	                   std::function<bool()> EmptyDeterminant, std::function<void()> OnEmptyMag,
	                   std::function<void()> OnNestedFire, std::function<void()> OnFreshFire);

	void FireStopCore(const EGunSelector& Selector, uint16& FireCount);

	void SwitchSelectorCore(const uint16& FireCount, EGunSelector& DesiredSelector, FTimerHandle& SelectorTimer,
	                        std::function<void()> OnUpdateSelector);

	void NestedFireCore(const EGunSelector& Selector, uint16& FireCount);

	void FreshFireCore(const EGunSelector& Selector, uint16& FireCount, FTimerHandle& FireTimer,
	                   std::function<void()> RepeatFireFunction);

	void FireCallback(uint16& FireCount, FTimerHandle& FireTimer, std::function<bool()> EmptyDeterminant,
	                  std::function<void()> OnEmpty, std::function<void()> OnSingleFire);

	void StopFireCore(uint16& FireCount, FTimerHandle& FireTimer);

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
