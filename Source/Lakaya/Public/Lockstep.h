// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Lockstep.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULockstep : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LAKAYA_API ILockstep
{
	GENERATED_BODY()

public:
	/**
	 * @brief 결정론적 락스텝 구현을 위해 서버와 클라이언트가 기다릴 시간입니다.
	 */
	inline static constexpr float LockstepDelay = 0.1f;

	/**
	 * @brief 인터페이스에 선언되어 직접 호출할 수 없는 락스텝 함수의 Execute_ 함수를 좀 더 편리하게 호출합니다.
	 * @param ExecuteFunction 실행하려는 'Execute_'함수
	 */
	void Execute(void (*ExecuteFunction)(UObject*, const float&));
	/**
	 * @brief 인터페이스에 선언되어 직접 호출할 수 없는 락스텝 함수의 Execute_ 함수를 좀 더 편리하게 호출합니다. 현재 이 함수는 IInteraction을 위해서만 사용됩니다.
	 * @param ExecuteFunction 실행하려는 'Execute_'함수
	 * @param Caller 인터렉션을 요청하는 플레이어의 폰
	 */
	void Execute(void (*ExecuteFunction)(UObject*, const float&, const FUniqueNetIdRepl&), const APawn* Caller);
};
