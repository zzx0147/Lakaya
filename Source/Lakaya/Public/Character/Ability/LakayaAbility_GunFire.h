// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueInterface.h"
#include "LakayaAbility.h"
#include "LakayaAbility_GunFire.generated.h"

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

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void NativeEndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                              const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                              bool bWasCancelled) override;

private:
	/** 클라이언트로 타겟 데이터가 전달되면 호출되는 이벤트 함수입니다. */
	void ServerOnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag GameplayTag);

	/** 사격 판정을 수행하여 타겟 데이터 핸들을 생성합니다. */
	FGameplayAbilityTargetDataHandle FireTrace() const;

	/**
	 * @brief 사격 판정을 시뮬레이션 하여 새로운 타겟 데이터 핸들을 생성합니다.
	 * @param TargetDataHandle 사격 판정을 시뮬레이션할 타겟 데이터 핸들입니다.
	 */
	FGameplayAbilityTargetDataHandle SimulateFireTrace(const FGameplayAbilityTargetDataHandle& TargetDataHandle) const;

	/** 아바타 액터나 오너 액터를 IAbilityGunFireInterface로 캐스트하여 반환합니다. */
	IAbilityGunFireInterface* FindGunFireInterface() const;

	/** HitResult들을 타겟 데이터 핸들로 변환시킵니다. */
	static void HitResultsToTargetDataHandle(const TArray<FHitResult>& HitResults,
	                                         FGameplayAbilityTargetDataHandle& TargetDataHandle);

	void ActivateFireMontageTask();

	void ExecuteFireCue(const FGameplayAbilityTargetDataHandle& TargetDataHandle);

	void ConsumeTargetData() const;

	UFUNCTION()
	void OnMontageEnded();

	/** 사격시에 적용될 게임플레이 큐입니다. */
	UPROPERTY(EditDefaultsOnly)
	FGameplayCueTag FireCueTag;

	/** 사격시에 재생될 애니메이션 몽타주입니다. */
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> FireMontage;

	/** 피격된 대상에게 적용할 게임플레이 이펙트입니다. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;

	/**
	 * 클라이언트가 전달한 사격의 결과를 신뢰하도록 설정합니다.
	 * 로컬 클라이언트에서 롤백이 발생할 여지가 사라지므로 어빌리티의 퍼포먼스는 향상되지만, 핵 사용이 용이해집니다.
	 */
	UPROPERTY(EditDefaultsOnly)
	uint8 bTrustClientHitResult : 1;

	FDelegateHandle TargetDataDelegateHandle;
};
