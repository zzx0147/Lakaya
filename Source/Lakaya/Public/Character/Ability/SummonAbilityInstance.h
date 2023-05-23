// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SummonAbilityInstance.generated.h"

// 소환된 스킬의 인스턴스입니다. 실질적인 스킬 처리는 이 클래스의 파생에서 이뤄집니다.
UCLASS(Abstract)
class LAKAYA_API ASummonAbilityInstance : public AActor
{
	GENERATED_BODY()

	friend class UCoolTimedSummonAbility;

public:
	explicit ASummonAbilityInstance(const FObjectInitializer& ObjectInitializer);

	// 스킬이 소환된 직후 서버에서 호출됩니다.
	virtual void OnSummoned();

protected:
	// OwningAbility에게 이 어빌리티가 소환되었음을 알립니다.
	void NotifyAbilitySummoned();
	
	// OwningAbility에게 이 어빌리티의 역할이 끝났음을 알립니다.
	void NotifyAbilityEnded();

private:
	// 이 어빌리티 인스턴스를 소유하는 어빌리티를 설정합니다.
	FORCEINLINE void SetOwningAbility(class UCoolTimedSummonAbility* const& Ability) { OwningAbility = Ability; }

	UPROPERTY(Replicated, Transient)
	TObjectPtr<UCoolTimedSummonAbility> OwningAbility;
};
