// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilityGunFireInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UAbilityGunFireInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LAKAYA_API IAbilityGunFireInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * @brief 사격 어빌리티에서 아바타 액터에게 현재 시점의 사격의 결과를 질의할 때 사용됩니다.
	 * @param HitResults 사격의 충돌 검사를 수행하여 만들어진 HitResult들이 여기에 담깁니다.
	 */
	UFUNCTION(BlueprintNativeEvent)
	void FireTrace(TArray<FHitResult>& HitResults);

	/**
	 * @brief 사격 어빌리티가 사격의 결과를 시뮬레이션할 때 사용됩니다.
	 * @param Direction 사격이 이뤄졌던 위치 및 방향을 지정합니다.
	 * @param HitResults 사격을 시뮬레이트하여 만들어진 HitResult들이 여기에 담깁니다.
	 */
	UFUNCTION(BlueprintNativeEvent)
	void SimulateFireTrace(const FTransform& Direction, TArray<FHitResult>& HitResults);
};
