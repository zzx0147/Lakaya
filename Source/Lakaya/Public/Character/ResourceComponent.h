// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ResourceComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UResourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UResourceComponent();

	/**
	 * @brief 자원 소모를 시도하는 정수타입 오버로딩입니다.
	 * @return 성공하면 true를 반환합니다.
	 */
	virtual bool TryCostResource(const uint64& Value);

	/**
	 * @brief 자원 소모를 시도하는 부동소수점 타입 오버로딩입니다.
	 * @return 성공하면 true를 반환합니다.
	 */
	virtual bool TryCostResource(const double& Value);

	// 강제로 자원을 소모하는 정수타입 오버로딩입니다.
	virtual void CostResource(const uint64& Value) PURE_VIRTUAL(UResourceComponent::CostResource,)

	// 강제로 자원을 소모하는 부동소수점 타입 오버로딩입니다.
	virtual void CostResource(const double& Value) PURE_VIRTUAL(UResourceComponent::CostResource,)

	// 자원을 일정량 충전하는 정수타입 오버로딩입니다.
	virtual void GainResource(const uint64& Value) PURE_VIRTUAL(UResourceComponent::GainResource,)

	// 자원을 일정량 충전하는 정수타입 오버로딩입니다.
	virtual void GainResource(const double& Value) PURE_VIRTUAL(UResourceComponent::GainResource,)

	/**
	 * @brief 매개변수로 넘겨진 값이 소모될 수 있을 만큼 자원이 충분한지 검사하는 정수타입 오버로딩입니다.
	 * @return 성공하면 true를 반환합니다.
	 */
	virtual bool IsEnough(const uint64& Value) const PURE_VIRTUAL(UResourceComponent::IsEnough, return false;)

	/**
	 * @brief 매개변수로 넘겨진 값이 소모될 수 있을 만큼 자원이 충분한지 검사하는 정수타입 오버로딩입니다.
	 * @return 성공하면 true를 반환합니다.
	 */
	virtual bool IsEnough(const double& Value) const PURE_VIRTUAL(UResourceComponent::IsEnough, return false;)

	virtual bool IsFull() const PURE_VIRTUAL(UResourceComponent::IsFull, return false;)

	// 자원을 모두 소진시킵니다.
	virtual void Clear() PURE_VIRTUAL(UResourceComponent::Clear,)

	// 자원을 초기화합니다.
	virtual void Reset() PURE_VIRTUAL(UResourceComponent::Reset,)

	// 캐릭터의 생존 상태가 변경되면 호출됩니다. 서버, 클라이언트 모두에서 호출됩니다.
	virtual void OnAliveStateChanged(const bool& AliveState);
};
