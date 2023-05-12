// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StopRemoteCallAbility.h"
#include "LockstepFireAbility.generated.h"

DECLARE_EVENT_OneParam(ULockstepFireAbility, FWantsToFireStateChangedSignature, bool)

USTRUCT()
struct FLockstepFireInfo
{
	GENERATED_BODY()

	// 격발이 이뤄지는 서버 시간입니다.
	UPROPERTY()
	float Time;

	UPROPERTY()
	FVector CameraLocation;

	UPROPERTY()
	FVector CameraForward;

	UPROPERTY()
	FVector ActorLocation;

	bool operator>(const FLockstepFireInfo& Other) const { return Time > Other.Time; }
	bool operator<(const FLockstepFireInfo& Other) const { return Time < Other.Time; }

	// 카메라에서 액터를 향하는 이동벡터를 가져옵니다.
	FORCEINLINE FVector GetCameraToActor() const { return CameraLocation - ActorLocation; }

	/**
	 * @brief 카메라의 위치에서 카메라가 바라보는 방향으로 일정 거리만큼 진행한 위치를 가져옵니다.
	 * @param Distance 카메라의 전방 방향으로 얼마나 진행할지 정의합니다.
	 * @return 카메라의 위치 + 카메라 전방 벡터 * Distance
	 */
	FORCEINLINE FVector GetCameraForward(const float& Distance) const
	{
		return CameraLocation + CameraForward * Distance;
	}

	/**
	 * @brief 카메라의 전방 백터에 액터의 위치를 투영한 위치에서 카메라의 전방 방향으로 일정 거리만큼 진행한 위치를 가져옵니다.
	 * @param Distance 카메라의 전방 방향으로 얼마나 진행할지 정의합니다.
	 * @return 카메라의 위치 + 카메라 전방 벡터 * (카메라의 전방 벡터에 카메라에서 액터까지의 이동벡터를 투영한 길이 + Distance)
	 */
	FORCEINLINE FVector GetProjectedForward(const float& Distance) const
	{
		return GetCameraForward(CameraForward.Dot(GetCameraToActor()) + Distance);
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API ULockstepFireAbility : public UStopRemoteCallAbility
{
	GENERATED_BODY()

public:
	ULockstepFireAbility();
	virtual void InitializeComponent() override;
	virtual void AbilityStart() override;
	virtual void AbilityStop() override;

protected:
	virtual void RequestStart_Implementation(const float& RequestTime) override;
	virtual void RequestStop_Implementation(const float& RequestTime) override;

	// 격발타이머의 핸들러입니다.
	virtual void FireTick();

	// 발사 가능 여부를 판별합니다.
	virtual bool ShouldFire();

	// 단위 격발을 정의합니다.
	virtual void SingleFire(const FLockstepFireInfo& FireInfo);

	// 발사에 실패하는 경우 호출됩니다.
	virtual void FailToFire();

	// 플레이어의 격발 의지를 업데이트합니다.
	virtual void SetWantsToFire(const bool& FireState);

	virtual void ClearFireTimer();

	// 능력 사용을 시작할지 여부를 조사합니다.
	virtual bool ShouldAbilityStart();

	virtual void LockstepCallback();

	virtual void InvokeFireEvent();

	virtual void UpdateLockstepTimer(const float& CurrentTime);

private:
	// 격발을 모든 인스턴스에게 전파합니다.
	UFUNCTION(NetMulticast, Reliable)
	void NotifyFire(const FLockstepFireInfo& FireInfo);

public:
	// 플레이어의 격발 의지가 변경되었을 때 호출됩니다.
	FWantsToFireStateChangedSignature OnWantsToFireChanged;

protected:
	// 격발 간격 시간을 정의합니다.
	UPROPERTY(EditAnywhere)
	float FireDelay;

	// 초기 격발 지연 시간을 지정합니다.
	UPROPERTY(EditAnywhere)
	float FirstFireDelay;

	// 카메라에서부터 충돌검사를 수행하지 않는 일정 거리를 지정합니다.
	UPROPERTY(EditAnywhere)
	float CameraDeadZone;

	// 캐릭터를 기준으로하는 사정거리를 정의합니다.
	UPROPERTY(EditAnywhere)
	float FireRange;

	// 피격시 적용할 기본 피해량을 지정합니다.
	UPROPERTY(EditAnywhere)
	float BaseDamage;

	// 몇 초간 지연된 이후 사격이 시작되는지 정의합니다.
	UPROPERTY(EditAnywhere)
	float LockstepDelay;

private:
	// 플레이어의 격발 의지를 나타냅니다.
	bool bWantsToFire;

	FTimerHandle FireTimer;
	TWeakObjectPtr<class UCameraComponent> Camera;
	FCollisionQueryParams CollisionQueryParams;
	TArray<FLockstepFireInfo> FireInfos;
	float RecentTopFireTime = FLT_MAX;
	FTimerHandle LockstepTimer;
};
