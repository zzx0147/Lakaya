// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitCustomImmunity.generated.h"

class ULakayaAbilityCustomImmunity;
/**
 * 
 */
UCLASS()
class LAKAYA_API UAbilityTask_WaitCustomImmunity : public UAbilityTask
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTaskCanceled);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnImmuneDelegate, const FGameplayEffectSpec&, Spec,
	                                             const ULakayaAbilityCustomImmunity*, CustomImmunity);

	UPROPERTY(BlueprintAssignable)
	FOnImmuneDelegate OnImmune;

	UPROPERTY(BlueprintAssignable)
	FTaskCanceled OnCanceled;

	UFUNCTION(BlueprintCallable,
		meta=(HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_WaitCustomImmunity* WaitCustomImmunity(UGameplayAbility* OwningAbility,
	                                                           ULakayaAbilityCustomImmunity* CustomImmunity,
	                                                           bool bTriggerOnce = false,
	                                                           bool bTriggerWhenSameClass = true);

	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void OnImmunedByCustomImmunity(const FGameplayEffectSpec& Spec,
	                               const ULakayaAbilityCustomImmunity* InCustomImmunity);

	TWeakObjectPtr<ULakayaAbilityCustomImmunity> CustomImmunity;
	FDelegateHandle OnImmunedByCustomImmunityHandle;
	bool bIsTriggerOnce;
	bool bOnlySameClass;
};
