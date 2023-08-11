// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LakayaAbility.h"
#include "LakayaAbility_ContinuousTrigger.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API ULakayaAbility_ContinuousTrigger : public ULakayaAbility
{
	GENERATED_BODY()

public:
	ULakayaAbility_ContinuousTrigger();
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION()
	void TryActivateTriggerAbility();

	UPROPERTY(EditAnywhere, DisplayName="트리거 대상 어빌리티")
	TSubclassOf<UGameplayAbility> TriggerAbilityClass;

	UPROPERTY(Replicated, Transient)
	FGameplayAbilitySpecHandle TriggerAbilityHandle;
};
