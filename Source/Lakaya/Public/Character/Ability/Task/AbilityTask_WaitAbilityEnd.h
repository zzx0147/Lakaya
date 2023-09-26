// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitAbilityEnd.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityEndedDelegate, const FAbilityEndedData&, AbilityEndedData);

/**
 * 
 */
UCLASS()
class LAKAYA_API UAbilityTask_WaitAbilityEnd : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable,
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_WaitAbilityEnd* WaitAbilityEnd(UGameplayAbility* InOwningAbility,
	                                                   const FGameplayAbilitySpecHandle& InAbilityHandle,
	                                                   const bool& bInInvokeOnce = false);

	UPROPERTY(BlueprintAssignable)
	FAbilityEndedDelegate OnAbilityEnded;

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void AbilityEnded(const FAbilityEndedData& AbilityEndedData);

	FDelegateHandle AbilityEndedDelegateHandle;
	FGameplayAbilitySpecHandle AbilityHandle;
	bool bInvokeOnce;
};
