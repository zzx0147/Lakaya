// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LakayaAbility_Zoom.h"
#include "LakayaAbility_ZoomInputContext.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API ULakayaAbility_ZoomInputContext : public ULakayaAbility_Zoom
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void NativeEndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                              const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                              bool bWasCancelled) override;

private:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<ULakayaInputContext> InputContext;
};
