// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LAKAYA_API IInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * @brief 인터렉션 시작을 서버에 요청합니다. 직접 호출하는 대신 Invoke 함수를 사용하여 호출하세요.
	 * @param Time 클라이언트에서 인터렉션을 시작한 시간입니다.
	 * @param CallerID 플레이어의 UniqueID입니다.
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void InteractionStart(const float& Time, APawn* Caller);

	virtual bool InteractionStart_Validate(const float& Time, APawn* Caller) { return true; }

	/**
	 * @brief 인터렉션 중단을 서버에 요청합니다. 직접 호출하는 대신 Invoke 함수를 사용하여 호출하세요.
	 * @param Time 클라이언트에서 인터렉션을 중단한 시간입니다.
	 * @param CallerID 플레이어의 UniqueID입니다.
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void InteractionStop(const float& Time, APawn* Caller);

	virtual bool InteractionStop_Validate(const float& Time, APawn* Caller) { return true; }


	/**
	 * @brief 인터페이스의 이벤트 함수를 호출하는 절차의 편의를 위해 만들어진 함수입니다.
	 * @param Func 호출하려는 이벤트 함수
	 * @param Caller 호출하는 APawn
	 */
	inline void Invoke(void (*Func)(UObject*, const float&, APawn*), APawn* Caller)
	{
		const auto Object = Cast<UObject>(this);
		Func(Object, Object->GetWorld()->GetGameState()->GetServerWorldTimeSeconds(), Caller);
	}

	// static void Execute_InteractionStart(UObject* Object, const float& X, APawn* Pawn);
	//
	// static void Execute_InteractionStop(UObject* Object, const float& X, APawn* Pawn);
};