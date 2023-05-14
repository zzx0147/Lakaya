// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleObjectPool.h"
#include "StartRemoteCallAbility.h"
#include "CoolTimedSummonAbility.generated.h"

DECLARE_EVENT_OneParam(UCoolTimedSummonAbility, FEnableTimeSignature, const float&)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UCoolTimedSummonAbility : public UStartRemoteCallAbility
{
	GENERATED_BODY()

public:
	UCoolTimedSummonAbility();
	virtual void AbilityStart() override;

protected:
	virtual void InitializeComponent() override;
	virtual void RequestStart_Implementation(const float& RequestTime) override;

	UFUNCTION()
	virtual void OnRep_EnableTime();

private:
	UFUNCTION()
	void OnAbilityInstanceEnded(class ASummonAbilityInstance* const& AbilityInstance);

public:
	// 스킬 사용가능시점이 변경된 경우 호출됩니다.
	FEnableTimeSignature OnEnableTimeChanged;

protected:
	// 스킬의 기본 쿨타임을 지정합니다.
	UPROPERTY(EditAnywhere)
	float CoolTime;

	// 소환될 스킬의 인스턴스입니다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<ASummonAbilityInstance> AbilityInstanceClass;

	// 액터에서 어느정도 떨어진 거리에서 어빌리티 인스턴스를 소환할지 지정합니다. bWantsTransformSet이 false인 경우 사용되지 않습니다.
	UPROPERTY(EditAnywhere)
	float SummonDistance;

	// 액터에서 어느정도 떨어진 거리까지 트레이스를 수행할지 지정합니다. bWantsTransformSet이 false인 경우 사용되지 않습니다.
	UPROPERTY(EditAnywhere)
	float SearchFromActor;

	// 초기 생성할 오브젝트 풀 크기를 지정합니다.
	UPROPERTY(EditAnywhere)
	uint8 ObjectPoolSize;

	// 어빌리티 인스턴스가 플레이어가 바라보는 방향으로 로케이션, 로테이션이 셋업되도록 합니다.
	UPROPERTY(EditAnywhere)
	bool bWantsTransformSet;

private:
	// 스킬이 사용가능해지는 시점을 의미합니다.
	UPROPERTY(ReplicatedUsing=OnRep_EnableTime, Transient)
	float EnableTime;

	FCollisionQueryParams CollisionQueryParams;
	TSimpleObjectPool<ASummonAbilityInstance> AbilityInstancePool;
};
