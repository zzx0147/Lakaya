// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SummonAbilityInstance.generated.h"

DECLARE_EVENT_OneParam(ASummonAbilityInstance, FAbilityEndedSignature, class ASummonAbilityInstance* const&)

// 소환된 스킬의 인스턴스입니다. 실질적인 스킬 처리는 이 클래스의 파생에서 이뤄집니다.
UCLASS()
class LAKAYA_API ASummonAbilityInstance : public AActor
{
	GENERATED_BODY()

public:
	explicit ASummonAbilityInstance(const FObjectInitializer& ObjectInitializer);

	// 스킬이 소환된 직후 서버에서 호출됩니다.
	virtual void OnSummoned();

protected:
	// OnAbilityEnded 이벤트를 브로드캐스트합니다.
	void BroadcastOnAbilityEnded();

public:
	// 스킬 시전이 종료되는 시점에 호출됩니다.
	FAbilityEndedSignature OnAbilityEnded;
};
