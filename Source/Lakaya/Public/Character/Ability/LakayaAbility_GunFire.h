// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueInterface.h"
#include "LakayaAbility.h"
#include "LakayaAbility_GunFire.generated.h"

class UAbilityComponent_FireTrace;
class IAbilityGunFireInterface;
/**
 * 
 */
UCLASS()
class LAKAYA_API ULakayaAbility_GunFire : public ULakayaAbility
{
	GENERATED_BODY()

public:
	ULakayaAbility_GunFire();

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	virtual FGameplayAbilityTargetDataHandle MakeTargetData_Implementation() override;
	virtual void OnTargetDataReceived_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	                                                 FGameplayTag GameplayTag) override;

private:
	/** 피격된 대상에게 적용할 게임플레이 이펙트입니다. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAbilityComponent_FireTrace> FireTraceComponentClass;

	TWeakObjectPtr<UAbilityComponent_FireTrace> FireTraceComponent;
	bool bIsFireTraceComponentAdded;
};
