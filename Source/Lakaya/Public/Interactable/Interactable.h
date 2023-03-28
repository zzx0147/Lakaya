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
	 * @brief 캐릭터가 서버에 인터렉션 시작을 요청하기 전에 로컬 기기에서 호출됩니다.
	 * 이 함수는 로컬 기기에서 실행되므로 게임모드를 찾거나 서버에서만 실행 가능한 로직을 실행하면 안됩니다.
	 * @param Caller 인터렉션을 요청한 캐릭터입니다.
	 */
	virtual void OnLocalInteractionBegin(APawn* Caller) { return; }

	/**
	 * @brief 서버에서 캐릭터의 인터렉션 요청이 수신되고 호출됩니다. 인터렉션 초기에 서버측에서 실행해야할 로직을 정의합니다.
	 * @param Time 캐릭터가 인터렉션을 시작한 시간입니다.
	 * @param Caller 인터렉션을 요청한 캐릭터입니다.
	 */
	virtual void OnServerInteractionBegin(const float& Time, APawn* Caller) = 0;

	/**
	 * @brief 캐릭터의 인터렉션 요청이 승인되어 인터렉션을 시작합니다. 이 함수는 서버에서 실행됩니다.
	 * @param Caller 인터렉션을 시작한 캐릭터입니다.
	 */
	virtual void OnInteractionStart(APawn* Caller) = 0;

	/**
	 * @brief 캐릭터가 인터렉션 중단을 요청하기 전에 로컬 기기에서 호출됩니다.
	 * @param Caller 인터렉션 중단을 요청한 캐릭터입니다.
	 */
	virtual void OnLocalInteractionStopBegin(APawn* Caller) { return; }

	/**
	 * @brief 서버가 캐릭터의 인터렉션 중단 요청을 수신하고 호출됩니다. 인터렉션 중단 시
	 * @param Time 캐릭터가 인터렉션을 중단한 시간입니다.
	 * @param Caller 인터렉션 중단을 요청한 캐릭터입니다.
	 */
	virtual void OnServerInteractionStopBegin(const float& Time, APawn* Caller) { return; }

	/** 
	 * @brief 캐릭터의 인터렉션 중단 요청이 승인되어 인터렉션을 중단합니다. 이 함수는 서버에서 실행됩니다.
	 * @param Caller 인터렉션을 중단한 캐릭터입니다.
	 */
	virtual void OnInteractionStop(APawn* Caller) { return; }

	/**
	 * @brief 인터렉션 중이던 캐릭터가 사망하면 호출됩니다. 이 함수는 서버측에서 호출됩니다.
	 * @param Caller 사망한 캐릭터입니다.
	 */
	virtual void OnCharacterDead(APawn* Caller) { return; }


	virtual void InteractionStart(const float& Time, APawn* Caller) { return; }
	virtual void InteractionStop(const float& Time, APawn* Caller) { return; }
};
