// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAbility.generated.h"


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

	// true로 설정하면 서버에게 플레이어가 능력 사용을 시작했음을 알립니다.
	UPROPERTY(EditAnywhere)
	bool bCanEverStartRemoteCall;

	// true로 설정하면 서버에게 플레이어가 능력 사용을 중단했음을 알립니다.
	UPROPERTY(EditAnywhere)
	bool bCanEverStopRemoteCall;

private:
	TWeakObjectPtr<UCameraComponent> CameraComponent;
	bool bRecentAliveState;
};
