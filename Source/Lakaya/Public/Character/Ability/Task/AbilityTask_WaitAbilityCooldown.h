// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitAbilityCooldown.generated.h"

/**
 * 대상 어빌리티가 종료된 이후 쿨다운을 기다리는 어빌리티 태스크입니다.
 */
UCLASS()
class LAKAYA_API UAbilityTask_WaitAbilityCooldown : public UAbilityTask
{
	GENERATED_BODY()

public:
	/**
	 * 대상 어빌리티의 쿨다운을 기다리는 어빌리티 태스크를 생성합니다.
	 * @param InOwningAbility 태스크를 소유하는 어빌리티입니다.
	 * @param InAbilityHandle 쿨다운을 기다릴 대상 어빌리티의 핸들입니다.
	 * @param bInInvokeOnce 한번만 기다릴지 여부를 결정합니다. true라면 OnCooldownEnded는 한 번만 호출되고 이 태스크는 바로 종료됩니다.
	 */
	static UAbilityTask_WaitAbilityCooldown* WaitAbilityCooldown(UGameplayAbility* InOwningAbility,
	                                                             const FGameplayAbilitySpecHandle& InAbilityHandle,
	                                                             const bool& bInInvokeOnce = false);

	/** 대상 어빌리티의 쿨다운이 종료되면 호출되는 델리게이트입니다. */
	FSimpleMulticastDelegate OnCooldownEnded;

protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

private:
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);

	UFUNCTION()
	void InvokeOnCooldownEnded();

	FDelegateHandle AbilityEndedDelegateHandle;
	FGameplayAbilitySpecHandle AbilityHandle;
	bool bInvokeOnce;
};
