// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAbility.generated.h"

UENUM()
enum class EResourceKind
{
	Bullet
};

USTRUCT()
struct FResourceCostData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EResourceKind ResourceKind;

	UPROPERTY(EditAnywhere)
	float Value;
};

DECLARE_EVENT_OneParam(UCharacterAbility, FEnableTimeSignature, const float&)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UCharacterAbility : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterAbility();
	virtual void InitializeComponent() override;

	// 능력 사용을 시작합니다. 이 함수는 클라이언트에서 호출됩니다.
	virtual void LocalAbilityStart() { return; }

	// 능력 사용을 중단합니다. 이 함수는 클라이언트에서 호출됩니다.
	virtual void LocalAbilityStop() { return; }

	/**
	 * @brief 서버로 클라이언트의 능력 사용요청 신호가 수신되면 호출됩니다.
	 * @param RequestTime 클라이언트가 능력 사용요청을 시도한 시간입니다.
	 */
	virtual void RemoteAbilityStart(const float& RequestTime) { return; }

	/**
	 * @brief 서버로부터 클라이언트의 능력 중단요청 신호가 수신되면 호출됩니다.
	 * @param RequestTime 클라이언트가 능력 사용중단을 요청한 시간입니다.
	 */
	virtual void RemoteAbilityStop(const float& RequestTime) { return; }

	// 캐릭터의 생존 상태가 업데이트될 때 호출됩니다. 서버, 클라이언트 모두에서 호출됩니다.
	virtual void OnAliveStateChanged(const bool& AliveState);

	bool CanStartRemoteCall();
	bool CanStopRemoteCall();

protected:
	// 서버에게 능력 사용 요청 신호를 보낼지 여부를 조사합니다.
	virtual bool ShouldStartRemoteCall();

	// 서버에게 능력 중단 요청 신호를 보낼지 여부를 조사합니다.
	virtual bool ShouldStopRemoteCall();

	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent.Get(); }

	template <class T = class UResourceComponent>
	FORCEINLINE T* GetResourceComponent() const { return Cast<T>(ResourceComponent); }

	/**
	 * @brief 액터에서 카메라가 바라보는 방향으로 움직인 월드공간의 위치를 가져옵니다.
	 * @param Distance 카메라가 바라보는 방향으로 얼마나 전진할지 지정합니다.
	 * @param FromActor 액터로부터 얼마나 떨어진 지점까지 트레이스를 수행할지 지정합니다.
	 * @param CollisionParams 트레이스 검색에서 사용할 옵션입니다.
	 */
	FVector GetDistancedToCameraDirection(const float& Distance, const float& FromActor,
	                                      const FCollisionQueryParams& CollisionParams) const;

	/**
	 * @brief 카메라에서 트레이스를 수행하여 충돌하는 점을 가져옵니다. 충돌하지 않으면 TraceEnd를 반환합니다.
	 * @param FromActor 액터로부터 얼마나 떨어진 지점까지 트레이스를 수행할지 지정합니다.
	 * @param CollisionQueryParams 트레이스 검색에서 사용할 옵션입니다.
	 */
	FVector GetCameraForwardTracePoint(const float& FromActor, const FCollisionQueryParams& CollisionQueryParams) const;

	/**
	 * @brief 카메라의 전방 방향에서 액터로부터 일정거리만큼 떨어진 위치를 가져옵니다.
	 * @param FromActor 액터로부터 얼마나 떨어진 위치인지 지정합니다.
	 */
	FVector GetCameraForwardPointFromActor(const float& FromActor) const;

	FVector GetNormalToCameraForwardTracePoint(const float& FromActor,
	                                           const FCollisionQueryParams& CollisionQueryParams) const;

	// 가장 최근 업데이트된 캐릭터의 생존 상태를 가져옵니다.
	FORCEINLINE const bool& GetAliveState() const { return bRecentAliveState; }

	float GetServerTime() const;

	/**
	 * @brief 캐릭터의 자원에 대해 소모를 시도합니다.
	 * @param ResourceKind 소모할 자원의 종류입니다.
	 * @param Value 자원을 얼마나 소모할지 지정합니다.
	 * @return 소모에 성공하면 true, 실패하는 경우 false를 반환합니다.
	 */
	bool CostResource(const EResourceKind& ResourceKind, const float& Value);

	/**
	 * @brief 여러가지 자원들을 동시에 소모를 시도합니다.
	 * @param CostArray 특정 자원을 얼마나 소모할지 나타내는 배열입니다.
	 * @return 소모에 성공하면 true, 실패하는 경우 false를 반환합니다.
	 */
	bool CostResource(const TArray<FResourceCostData>& CostArray);

	UFUNCTION()
	virtual void OnRep_EnableTime();

	// 최종적으로 적용할 쿨타임을 가져옵니다.
	virtual float GetCoolTime() { return BaseCoolTime; }

	// 쿨타임을 적용합니다. 반드시 서버측에서 호출되어야 합니다.
	virtual void ApplyCoolTime();

	// EnableTime과 비교하여 스킬사용이 가능한 시간인지 체크합니다.
	bool IsEnableTime(const float& Time) const { return EnableTime <= Time; }

public:
	// 스킬 사용가능 시점이 변경될 때 호출됩니다.
	FEnableTimeSignature OnEnableTimeChanged;

protected:
	// true로 설정하면 ShouldStartRemoteCall()이 호출되며, 이 함수가 true를 반환할 때 서버에서 RemoteAbilityStart()가 호출됩니다.
	UPROPERTY(EditAnywhere)
	bool bCanEverStartRemoteCall;

	// true로 설정하면 ShouldStopRemoteCall()이 호출되며, 이 함수가 true를 반환할 때 서버에서 RemoteAbilityStop()가 호출됩니다.
	UPROPERTY(EditAnywhere)
	bool bCanEverStopRemoteCall;

	// 아무런 효과가 없을 때 적용될 스킬의 기본 쿨타임입니다.
	UPROPERTY(EditAnywhere)
	float BaseCoolTime;

private:
	UPROPERTY(ReplicatedUsing=OnRep_EnableTime, Transient)
	float EnableTime;

	TWeakObjectPtr<UCameraComponent> CameraComponent;
	TWeakObjectPtr<class UResourceComponent> ResourceComponent;
	bool bRecentAliveState;
};
