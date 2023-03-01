// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Lockstep.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public ULockstep
{
	GENERATED_BODY()
};

/**
 * 
 */
class LAKAYA_API IInteractable : public ILockstep
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * @brief 인터렉션 시작을 서버에 요청합니다. 직접 호출하지 않고, Execute(IInteractable::Execute_InteractionStart)와 같이 호출합니다.
	 * @param Time 클라이언트에서 인터렉션을 시작한 시간입니다.
	 * @param CallerId 플레이어의 UniqueID입니다.
	 */
	UFUNCTION(Server, Reliable)
	virtual void InteractionStart(const float& Time, const FUniqueNetIdRepl& CallerId);

	/**
	 * @brief 인터렉션 중단을 서버에 요청합니다. 직접 호출하지 않고, Execute(IInteractable::Execute_InteractionStop)와 같이 호출합니다.
	 * @param Time 클라이언트에서 인터렉션을 중단한 시간입니다.
	 * @param CallerId 플레이어의 UniqueID입니다.
	 */
	UFUNCTION(Server, Reliable)
	virtual void InteractionStop(const float& Time, const FUniqueNetIdRepl& CallerId);
};
