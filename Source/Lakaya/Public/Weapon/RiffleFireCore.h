// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FocusableCharacter.h"
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
	void FireStartCore(FTimerHandle& FireTimer,
	                   std::function<bool()> FocusableDeterminant, std::function<void()> OnContinuousFire, std::function<void()> OnFreshFire);

	void FireStopCore(const EGunSelector& Selector, uint16& FireCount, const bool& IsSimulated);

	void SwitchSelectorCore(EGunSelector& DesiredSelector, FTimerHandle& SelectorTimer,
	                        std::function<void()> OnUpdateSelector, std::function<bool()> NotFocusableDeterminant);

	void ContinuousFireCore(const EGunSelector& Selector, uint16& FireCount);

	void FreshFireCore(const EGunSelector& Selector, uint16& FireCount, FTimerHandle& FireTimer,
	                   std::function<void()> RepeatFireFunction);

	void FireCallback(uint16& FireCount, FTimerHandle& FireTimer, std::function<bool()> EmptyDeterminant,
	                  std::function<void()> OnEmpty, std::function<void()> OnSingleFire, std::function<void()> OnFirePreEnding);
	
	void UpdateSelector(EGunSelector& DesiredSelector, EGunSelector& Selector, const bool& IsSimulated);
	
	UFUNCTION()
	virtual void OnRep_Character() { return; }

private:
	void SetFireCount(const EGunSelector& Selector, uint16& FireCount);

protected:
	UPROPERTY(Replicated)
	TWeakObjectPtr<class UGunComponent> GunComponent;

	UPROPERTY(ReplicatedUsing=OnRep_Character)
	TWeakObjectPtr<class AFocusableCharacter> Character;

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
