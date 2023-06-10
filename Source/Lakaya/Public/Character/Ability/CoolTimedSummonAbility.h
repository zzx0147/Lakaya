// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbility.h"
#include "SimpleObjectPool.h"
#include "Occupation/PlayerTeam.h"
#include "CoolTimedSummonAbility.generated.h"

enum class EAbilityInstanceState;

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
	void OnAbilityInstanceStateChanged(const EAbilityInstanceState& InstanceState,
	                                   class ASummonAbilityInstance* const& AbilityInstance);

	// 어빌리티 인스턴스 소환 시, 소환의 기준 위치를 선정할 때 사용되는 씬 컴포넌트를 지정합니다.
	UFUNCTION(BlueprintSetter)
	void SetBasisComponent(USceneComponent* NewComponent);

protected:
	// 소환될 어빌리티 인스턴스의 클래스를 지정합니다.
	UPROPERTY(EditAnywhere)
	TSubclassOf<ASummonAbilityInstance> AbilityInstanceClass;

	// 액터에서 어느정도 떨어진 거리에서 어빌리티 인스턴스를 소환할지 지정합니다.
	UPROPERTY(EditAnywhere)
	float SummonDistance;

	// 액터에서 어느정도 떨어진 거리까지 트레이스를 수행할지 지정합니다.
	UPROPERTY(EditAnywhere)
	float SearchFromActor;

	// 초기 생성할 오브젝트 풀 크기를 지정합니다.
	UPROPERTY(EditAnywhere)
	uint8 ObjectPoolSize;

	// 어빌리티 인스턴스 소환시 소모할 자원들을 지정합니다.
	UPROPERTY(EditAnywhere)
	TArray<FResourceCostData> ResourceCost;

private:
	UPROPERTY(BlueprintSetter=SetBasisComponent)
	TObjectPtr<USceneComponent> BasisComponent;

	FCollisionQueryParams CollisionQueryParams;
	TSimpleObjectPool<ASummonAbilityInstance> AbilityInstancePool;
};
