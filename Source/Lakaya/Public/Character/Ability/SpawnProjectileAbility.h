// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StartRemoteCallAbility.h"
#include "SpawnProjectileAbility.generated.h"

DECLARE_EVENT_OneParam(USpawnProjectileAbility, FEnableTimeSignature, const float&)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API USpawnProjectileAbility : public UStartRemoteCallAbility
{
	GENERATED_BODY()

public:
	USpawnProjectileAbility();
	virtual void AbilityStart() override;

protected:
	virtual void RequestStart_Implementation(const float& RequestTime) override;

	UFUNCTION()
	virtual void OnRep_EnableTime();

public:
	// 스킬 사용가능시점이 변경된 경우 호출됩니다.
	FEnableTimeSignature OnEnableTimeChanged;

protected:
	// 스킬의 기본 쿨타임을 지정합니다.
	UPROPERTY(EditAnywhere)
	float CoolTime;

	// 스킬 사용시 스폰될 투사체 클래스를 지정합니다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ASingleDamageProjectile> ProjectileClass;

	// 액터의 전방으로 어느정도 떨어진 거리에서 투사체를 스폰할지 지정합니다.
	UPROPERTY(EditAnywhere)
	float SpawnDistance;

	// 투사체가 사용할 기본 데미지를 지정합니다.
	UPROPERTY(EditAnywhere)
	float BaseDamage;

private:
	// 스킬이 사용가능해지는 시점을 의미합니다.
	UPROPERTY(ReplicatedUsing=OnRep_EnableTime, Transient)
	float EnableTime;
};
