// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTagQuery.h"
#include "AbilityTask_WaitGameplayTagQueryWrapper.generated.h"

/**
 * UAbilityTask_WaitGameplayTagQuery 클래스를 C++에서 사용할 수 있도록 래핑한 클래스입니다.
 */
UCLASS()
class LAKAYA_API UAbilityTask_WaitGameplayTagQueryWrapper : public UAbilityTask_WaitGameplayTagQuery
{
	GENERATED_BODY()

public:
	static UAbilityTask_WaitGameplayTagQueryWrapper* WaitGameplayTagQuery(
		UGameplayAbility* OwningAbility, const FGameplayTagQuery TagQuery,
		const AActor* InOptionalExternalTarget = nullptr,
		const EWaitGameplayTagQueryTriggerCondition TriggerCondition = EWaitGameplayTagQueryTriggerCondition::WhenTrue,
		const bool bOnlyTriggerOnce = false);

	FWaitGameplayTagQueryDelegate& OnTriggered() { return Triggered; }

private:
	void SetExternalTargetWrapper(const AActor* Actor);
};
