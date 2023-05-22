// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbility.h"
#include "SimpleObjectPool.h"
#include "CoolTimedSummonAbility.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UCoolTimedSummonAbility : public UCharacterAbility
{
	GENERATED_BODY()

public:
	UCoolTimedSummonAbility();

protected:
	virtual void InitializeComponent() override;
	virtual bool ShouldStartRemoteCall() override;
	virtual void RemoteAbilityStart(const float& RequestTime) override;

private:
	UFUNCTION()
	void OnAbilityInstanceEnded(class ASummonAbilityInstance* const& AbilityInstance);

protected:
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

	// 어빌리티 인스턴스 소환시 소모할 자원들을 지정합니다.
	UPROPERTY(EditAnywhere)
	TArray<FResourceCostData> ResourceCost;

private:
	FCollisionQueryParams CollisionQueryParams;
	TSimpleObjectPool<ASummonAbilityInstance> AbilityInstancePool;
};
