// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RegisterAbilityInterface.generated.h"

class UAbilitySystemComponent;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URegisterAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LAKAYA_API IRegisterAbilityInterface
{
	GENERATED_BODY()

public:
	/**
	 * @brief 어빌리티 시스템에 어빌리티를 등록하라는 지시입니다.
	 */
	virtual void GiveAbilities(UAbilitySystemComponent* InAbilitySystem) = 0;

	/**
	 * @brief 어빌리티 시스템에서 등록했던 어빌리티를 제거하라는 지시입니다.
	 */
	virtual void ClearAbilities() = 0;
};
