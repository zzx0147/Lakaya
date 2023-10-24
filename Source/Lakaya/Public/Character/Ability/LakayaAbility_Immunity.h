// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Ability/LakayaAbility.h"
#include "LakayaAbility_Immunity.generated.h"


UCLASS()
class LAKAYA_API ULakayaAbility_Immunity : public ULakayaAbility
{
	GENERATED_BODY()

protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	
	virtual void NativeEndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                              const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                              bool bWasCancelled) override;

	virtual void OnImmunityBlockGameplayEffect(const FGameplayEffectSpec& BlockedSpec,
	                                     const FActiveGameplayEffect* ImmunityGameplayEffect);

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OnImmunityBlockGameplayEffect")
	void K2_OnImmunityBlockGameplay(const FGameplayEffectSpec& BlockedSpec,
											 const FActiveGameplayEffect ImmunityGameplayEffect);

	//K2_OnImmunityBlockGameplay 함수가 반응할 이펙트 목록입니다. 해당 배열에 등록된 면역 이펙트로 면역된 경우에만 K2_OnImmunityBlockGameplay 함수가 호출됩니다. 이펙트의 호출은 별개로 해야합니다.
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayEffect>> ImmunityEffectArray;

	UFUNCTION(BlueprintCallable)
	float GetSetByCallerMagnitude(const FGameplayEffectSpec& Spec ,FGameplayTag DataTag, bool WarnIfNotFound, float DefaultIfNotFound);

	UFUNCTION(BlueprintCallable)
	bool GetModifiedAttributeValue(const FGameplayEffectSpec& Spec, FGameplayAttribute TargetAttribute, float& OutMagnitude);
	
private:
	FDelegateHandle ImmunityDelegateHandle;
};
