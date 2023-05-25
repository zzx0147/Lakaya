// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbility.h"
#include "SimpleObjectPool.h"
#include "Occupation/PlayerTeam.h"
#include "CoolTimedSummonAbility.generated.h"

DECLARE_EVENT_OneParam(UCoolTimedSummonAbility, FPerformTimeSignature, const float&)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UCoolTimedSummonAbility : public UCharacterAbility
{
	GENERATED_BODY()

public:
	UCoolTimedSummonAbility();
	virtual void SetTeam(const EPlayerTeam& Team) override;

protected:
	virtual void InitializeComponent() override;
	virtual bool ShouldStartRemoteCall() override;
	virtual void RemoteAbilityStart(const float& RequestTime) override;

public:
	void GetSummonLocationAndRotation(FVector& Location, FRotator& Rotator) const;
	void NotifyPerformTime(const float& Time);
	void NotifyAbilityInstanceCollapsed(class ASummonAbilityInstance* const& AbilityInstance);

	// 소환된 스킬이 실제로 언제 실행되는지 알려진 뒤에 호출됩니다. 매개변수로 스킬이 실행되는 시간을 받습니다.
	FPerformTimeSignature OnPerformTimeNotified;

protected:
	// 소환될 어빌리티 인스턴스의 클래스를 지정합니다.
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

	// 어빌리티 인스턴스 소환시 소모할 자원들을 지정합니다.
	UPROPERTY(EditAnywhere)
	TArray<FResourceCostData> ResourceCost;

	// 어빌리티 인스턴스 소환시 총구에서 재생될 나이아가라 시스템을 지정합니다.
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* MuzzleNiagaraSystem;

private:
	FCollisionQueryParams CollisionQueryParams;
	TSimpleObjectPool<ASummonAbilityInstance> AbilityInstancePool;
	TWeakObjectPtr<class UArrowComponent> MuzzleComponent;
	TWeakObjectPtr<class UNiagaraComponent> MuzzleNiagara;
};
