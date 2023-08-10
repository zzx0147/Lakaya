// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LakayaAbility.h"
#include "LakayaAbility_InputContext.generated.h"

class ULakayaInputContext;
/**
 * 
 */
UCLASS()
class LAKAYA_API ULakayaAbility_InputContext : public ULakayaAbility
{
	GENERATED_BODY()

public:
	ULakayaAbility_InputContext();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void NativeEndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                              const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                              bool bWasCancelled) override;

private:
	/** ActorInfo를 통해 입력 컨텍스트를 추가합니다. */
	void AddMappingContext(const FGameplayAbilityActorInfo* ActorInfo);

	/** ActorInfo를 통해 입력 컨텍스트를 제거합니다. */
	void RemoveMappingContext(const FGameplayAbilityActorInfo* ActorInfo);

	/** 이 어빌리티가 활성화되는 동안 사용될 입력 컨텍스트를 지정합니다. */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<ULakayaInputContext> InputContext;
};
