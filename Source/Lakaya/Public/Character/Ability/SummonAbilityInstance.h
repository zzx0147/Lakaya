// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Occupation/PlayerTeam.h"
#include "SummonAbilityInstance.generated.h"

UENUM()
enum class EAbilityInstanceState
{
	// 어빌리티 인스턴스의 행동이 완전이 종료되어 사용되지 않는 중임을 나타냅니다.
	Collapsed,

	// 어빌리티 인스턴스의 액션이 실행되기 전 준비단계를 나타냅니다.
	Ready,

	// 어빌리티 인스턴스의 액션이 실행중인 단계를 나타냅니다.
	Perform,

	// 어빌리티 인스턴스의 액션이 종료되고, 이펙트를 재생중이거나 어떤 다른 행동을 취하는 중임을 의미합니다.
	Ending
};

// 소환된 스킬의 인스턴스입니다. 실질적인 스킬 처리는 이 클래스의 파생에서 이뤄집니다.
UCLASS(Abstract)
class LAKAYA_API ASummonAbilityInstance : public AActor
{
	GENERATED_BODY()

public:
	explicit ASummonAbilityInstance(const FObjectInitializer& ObjectInitializer);

	// 이 어빌리티 인스턴스를 소유하는 어빌리티를 설정합니다.
	FORCEINLINE void SetOwningAbility(class UCoolTimedSummonAbility* const& Ability) { OwningAbility = Ability; }

	virtual void SetAbilityInstanceState(const EAbilityInstanceState& DesireState);
	FORCEINLINE const EAbilityInstanceState& GetInstanceState() const { return AbilityInstanceState; }
	virtual void SetTeam(const EPlayerTeam& Team) { return; }

protected:
	template <class T = UCoolTimedSummonAbility>
	T* GetOwningAbility() const { return Cast<T>(OwningAbility); }

	UFUNCTION()
	virtual void OnRep_AbilityInstanceState();

	virtual void HandleAbilityInstanceReady();
	virtual void HandleAbilityInstancePerform() { return; }
	virtual void HandleAbilityInstanceEnding();
	virtual void HandleAbilityInstanceCollapsed();

	virtual void PerformTimerHandler();
	virtual void CollapseTimerHandler();

	float GetServerTime() const;
	FORCEINLINE const float& GetAbilityTime() const { return AbilityTime; }

	// 몇 초 뒤에 Perform으로 진입할지 지정합니다. 0이거나 음수면 수동으로 Perform으로 바꿔줘야 합니다.
	UPROPERTY(EditAnywhere)
	float PerformDelay;

	// 몇 초 뒤에 Collapsed로 진입할지 지정합니다. 0이거나 음수면 수동으로 Collapsed로 바꿔줘야 합니다.
	UPROPERTY(EditAnywhere)
	float CollapseDelay;

private:
	UPROPERTY(ReplicatedUsing=OnRep_AbilityInstanceState, Transient)
	EAbilityInstanceState AbilityInstanceState;

	UPROPERTY(Replicated, Transient)
	float AbilityTime;

	UPROPERTY(Replicated, Transient)
	TObjectPtr<UCoolTimedSummonAbility> OwningAbility;

	FTimerHandle PerformTimer;
	FTimerHandle CollapseTimer;
};
