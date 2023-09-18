// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	virtual FGameplayAbilityTargetDataHandle MakeTargetData_Implementation() override;
	virtual void OnTargetDataReceived_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	                                                 FGameplayTag GameplayTag) override;

private:
	/** 피격된 대상에게 적용할 게임플레이 이펙트입니다. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;

	/** 사격 트레이스를 구현하는 액터 컴포넌트 클래스를 지정합니다. 이 클래스가 아바타 액터에 있다면 그것을 사용하고, 없다면 추가합니다. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAbilityComponent_FireTrace> FireTraceComponentClass;

	TWeakObjectPtr<UAbilityComponent_FireTrace> FireTraceComponent;
	bool bIsFireTraceComponentAdded;
};
