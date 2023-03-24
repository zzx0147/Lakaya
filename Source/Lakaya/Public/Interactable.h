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
	 * @brief 캐릭터가 이 액터와 인터렉션을 시작했을 때 호출됩니다.
	 * @param Time 캐릭터가 인터렉션을 시작한 시간입니다.
	 * @param Caller 인터렉션을 요청한 캐릭터입니다.
	 */
	virtual void InteractionStart(const float& Time, APawn* Caller);
	
	/**
	 * @brief 캐릭터가 인터렉션을 중단한 경우에 호출됩니다.
	 * @param Time 캐릭터가 인터렉션을 중단한 시간입니다.
	 * @param Caller 인터렉션을 중단한 캐릭터입니다.
	 */
	virtual void InteractionStop(const float& Time, APawn* Caller);
};
