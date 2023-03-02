// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
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
	/**
	 * @brief 이벤트 함수 호출의 편의를 위해 선언된 함수입니다.
	 * @param Func 실행하려는 Execute_ 함수
	 */
	void Invoke(void (*Func)(UObject*, const float&));

	/**
	 * @brief 컴포넌트에 데이터를 전달합니다.
	 * @param Data 컴포넌트에 전달될 데이터입니다.
	 */
	virtual void SetupData(const FTableRowBase* Data) { return; }
};
