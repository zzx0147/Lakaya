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

protected:
	virtual FGameplayAbilityTargetDataHandle MakeTargetData_Implementation() override;
	virtual void OnTargetDataReceived_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	                                                 FGameplayTag GameplayTag) override;

private:
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
	                                         FGameplayAbilityTargetDataHandle& OutTargetDataHandle);

	/** 피격된 대상에게 적용할 게임플레이 이펙트입니다. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;

	/**
	 * 클라이언트가 전달한 사격의 결과를 신뢰하도록 설정합니다.
	 * 로컬 클라이언트에서 롤백이 발생할 여지가 사라지므로 어빌리티의 퍼포먼스는 향상되지만, 핵 사용이 용이해집니다.
	 */
	UPROPERTY(EditDefaultsOnly)
	uint8 bTrustClientHitResult : 1;
};
