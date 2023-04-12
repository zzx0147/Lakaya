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
	 * @brief 이 액터와 인터렉션을 시작합니다. 이 함수는 서버에서 호출됩니다.
	 * @param Time 클라이언트에서 이 액터와 인터렉션을 시작한 시간입니다.
	 * @param Caller 인터렉션을 요청한 캐릭터입니다.
	 */
	virtual void OnInteractionStart(const float& Time, APawn* Caller) = 0;

	/**
	 * @brief 이 액터와 인터렉션을 중단합니다. 이 함수는 서버에서 호출됩니다.
	 * @param Time 클라이언트에서 이 액터와 인터렉션을 중단한 시간입니다.
	 * @param Caller 인터렉션 중단을 요청한 캐릭터입니다.
	 */
	virtual void OnInteractionStop(const float& Time, APawn* Caller) { return; }

	/**
	 * @brief 인터렉션 중이던 캐릭터가 사망하면 호출됩니다. 이 함수는 서버측에서 호출됩니다.
	 * @param Caller 사망한 캐릭터입니다.
	 */
	virtual void OnCharacterDead(APawn* Caller) { return; }
};
