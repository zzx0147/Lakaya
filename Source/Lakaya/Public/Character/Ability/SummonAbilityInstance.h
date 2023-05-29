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

	// 어빌리티 인스턴스가 Perform이후 Action을 위해 준비단계에 들어갑니다.
	ReadyForAction,

	// 어빌리티 인스턴스가 Perform 외에 추가적으로 별도의 특별한 액션을 구현하는 경우 사용됩니다.
	Action,

	// 어빌리티 인스턴스의 액션이 종료되고, 이펙트를 재생중이거나 어떤 다른 행동을 취하는 중임을 의미합니다.
	Ending
};

DECLARE_EVENT_TwoParams(ASummonAbilityInstance, FAbilityInstanceStateSignature, const EAbilityInstanceState&,
                        class ASummonAbilityInstance* const&)

// 소환된 스킬의 인스턴스입니다. 실질적인 스킬 처리는 이 클래스의 파생에서 이뤄집니다.
UCLASS(Abstract)
class LAKAYA_API ASummonAbilityInstance : public AActor
{
	GENERATED_BODY()

public:
	explicit ASummonAbilityInstance(const FObjectInitializer& ObjectInitializer);

	// 이 어빌리티 인스턴스를 소유하는 어빌리티를 설정합니다.
	void SetOwningAbility(class UCoolTimedSummonAbility* const& Ability);

	// 어빌리티 인스턴스의 상태를 변경합니다.
	virtual void SetAbilityInstanceState(const EAbilityInstanceState& DesireState);

	// 어빌리티 인스턴스의 현재 상태를 가져옵니다.
	FORCEINLINE const EAbilityInstanceState& GetInstanceState() const { return AbilityInstanceState; }

	// 어빌리티 인스턴스에게 팀 정보를 셋업합니다.
	virtual void SetTeam(const EPlayerTeam& Team) { return; }

	/**
	 * @brief 어빌리티 인스턴스의 시간값을 가져옵니다. 이 시간값은 어빌리티 인스턴스의 상태에 따라 여러가지 용도로 쓰입니다.
	 * 일례로 Ready 상태에서 PerformDelay가 0보다 큰 값으로 설정된 경우 이 시간값은 Perform 상태로 변경되는 서버 시간을 의미합니다.
	 */
	FORCEINLINE const float& GetAbilityTime() const { return AbilityTime; }

protected:
	// 이 어빌리티 인스턴스를 소환한 어빌리티를 가져옵니다.
	template <class T = UCoolTimedSummonAbility>
	T* GetOwningAbility() const { return Cast<T>(OwningAbility); }

	UFUNCTION()
	virtual void OnRep_AbilityInstanceState();

	UFUNCTION()
	virtual void OnRep_OwningAbility();

	virtual void HandleAbilityInstanceReady();
	virtual void HandleAbilityInstancePerform() { return; }
	virtual void HandleAbilityInstanceReadyForAction() { return; }
	virtual void HandleAbilityInstanceAction() { return; }
	virtual void HandleAbilityInstanceEnding();
	virtual void HandleAbilityInstanceCollapsed() { return; }

	/**
	 * @brief AbilityTime을 통해 Perform 상태로 변경되는 시점을 유추하여 Perform으로 넘어가는 시점에 호출되는 함수입니다.
	 * 서버와 클라이언트 모두에서 동시에 호출되도록 만들어졌지만, 네트워크 환경이 열악한 환경에서는 클라이언트에서 이 함수가 제대로 호출되지 않을 수 있습니다.
	 */
	virtual void PerformTimerHandler();
	virtual void CollapseTimerHandler();

	float GetServerTime() const;

public:
	// 어빌리티 인스턴스의 상태가 변경되면 호출됩니다. 매개변수로 변경된 어빌리티 인스턴스의 상태와 이 어빌리티 인스턴스의 포인터를 받습니다.
	FAbilityInstanceStateSignature OnAbilityInstanceStateChanged;

protected:
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

	UPROPERTY(ReplicatedUsing=OnRep_OwningAbility, Transient)
	TWeakObjectPtr<UCoolTimedSummonAbility> OwningAbility;

	FTimerHandle PerformTimer;
	FTimerHandle CollapseTimer;
};
