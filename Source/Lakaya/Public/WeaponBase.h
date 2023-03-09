// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "queue"
#include "GameFramework/GameStateBase.h"
#include "WeaponBase.generated.h"


/**
 * 
 */
UCLASS(Config=Game, Abstract)
class LAKAYA_API UWeaponBase : public UObject
{
	GENERATED_BODY()

	struct FEventInfoStruct
	{
		inline bool operator<(const FEventInfoStruct& Other) const { return ExecutionTime < Other.ExecutionTime; }
		inline bool operator>(const FEventInfoStruct& Other) const { return ExecutionTime > Other.ExecutionTime; }

		float ExecutionTime;
		uint8 EventNumber;
	};

public:
	virtual bool IsNameStableForNetworking() const override { return false; }
	virtual bool IsSupportedForNetworking() const override { return true; }

	UFUNCTION(NetMulticast, Reliable)
	void SetupData(const FName& RowName);

	inline const bool& GetIsEnabled() const { return bIsEnabled; }
	inline void SetEnabled(const bool& Enabled) { bIsEnabled = Enabled; }

protected:
	virtual void SetupData_Implementation(const FName& RowName) { return; }
	inline float GetServerTime() { return GetWorld()->GetGameState()->GetServerWorldTimeSeconds(); }

	/**
	 * @brief 락스텝 이벤트를 등록합니다.
	 * @param EventNumber 이벤트를 구분하는 값입니다.
	 * @param RequestTime 최초 이벤트가 요청된 시간입니다.
	 */
	void ApplyEvent(const uint8& EventNumber, const float& RequestTime);

	/**
	 * @brief 이벤트가 실행될 시간이 도래하면 호출됩니다.
	 * @param EventNumber 이벤트를 구분하는 값입니다.
	 */
	virtual void ExecuteEvent(const uint8& EventNumber) { return; }

	/**
	 * @brief 클라이언트로부터 요청된 이벤트가 너무 늦게 도착한 경우 호출됩니다.
	 * @param EventNumber 이벤트를 구분하는 값입니다.
	 * @param RequestTime 최초 이벤트가 요청된 시간입니다.
	 * @param CurrentTime 현재 시간입니다.
	 */
	virtual void ExecuteLateEvent(const uint8& EventNumber, const float& RequestTime, const float& CurrentTime);

private:
	void EventTimerCallback();
	void EnqueueSetTimer(const FEventInfoStruct&& EventInfo);

protected:
	UPROPERTY(Config)
	float LockstepDelay = 0.1f;

private:
	std::priority_queue<FEventInfoStruct, std::vector<FEventInfoStruct>, std::greater<FEventInfoStruct>> PriorityQueue;
	FTimerHandle EventTimer;
	float CurrentEventExecutionTime = MAX_flt;
	bool bIsEnabled = true;
};
