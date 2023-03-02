// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponBase.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponBase : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief 무기의 여러가지 기능을 구현하는 컴포넌트들의 공통된 부모입니다.
 */
class LAKAYA_API IWeaponBase
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	void Invoke(void (*Func)(UObject*, const float&));
};
