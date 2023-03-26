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
	void FireStartCore(FTimerHandle& FireTimer, const EFocusContext& FocusContext,
	                   std::function<void()> OnContinuousFire, std::function<void()> OnFreshFire,
	                   std::function<void()> OnElse = nullptr);

	void FireStopCore(const EGunSelector& Selector, uint16& FireCount, const EFocusContext& FocusContext);

	void SwitchSelectorCore(EGunSelector& DesiredSelector, FTimerHandle& SelectorTimer,
	                        const EFocusContext& FocusContext, std::function<void()> OnUpdateSelector);

	void FreshFireCore(const EGunSelector& Selector, uint16& FireCount, FTimerHandle& FireTimer,
	                   std::function<void()> RepeatFireFunction);

	void FireCallback(uint16& FireCount, FTimerHandle& FireTimer, const EFocusContext& FocusContext,
	                  std::function<bool()> EmptyPredicate, std::function<void()> OnEmpty,
	                  std::function<void()> OnSingleFire);

	void UpdateSelector(EGunSelector& DesiredSelector, EGunSelector& Selector, const EFocusContext& FocusContext);

	void SetFireCount(const EGunSelector& Selector, uint16& FireCount);

private:
	virtual FColor GetDebugColor(const EFocusContext& FocusContext);

protected:
	UFUNCTION()
	virtual void OnRep_Character() { return; }

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
