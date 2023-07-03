// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StatPlayerState.h"
#include "BuffPlayerState.generated.h"

DECLARE_EVENT_OneParam(ABuffPlayerState, FKillLevelSignature, const uint8&)

DECLARE_EVENT_OneParam(ABuffPlayerState, FBuffSignature, const FName&)

UCLASS()
class LAKAYA_API ABuffPlayerState : public AStatPlayerState
{
	GENERATED_BODY()

public:
	virtual void IncreaseKillStreak() override;

protected:
	virtual void OnPawnSetCallback(APlayerState* Player, APawn* NewPawn, APawn* OldPawn) override;

public:
	/**
	 * @brief 플레이어에게 버프를 적용합니다.
	 * @param BuffName 적용할 버프의 이름입니다. 이 값을 통해 데이터테이블에서 버프를 찾아 적용합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void ApplyBuff(const FName& BuffName);

	/**
	 * @brief 플레이어에게 적용된 버프를 즉시 해제합니다. 제거하려는 버프가 없는 경우 아무 일도 일어나지 않습니다.
	 * @param BuffName 해제할 버프의 이름입니다.
	 */
	UFUNCTION(BlueprintCallable)
	void ExpireBuff(const FName& BuffName);

protected:
	UFUNCTION()
	virtual void OnRep_KillStreakBuffLevel();

	void SetKillStreakBuffLevel(const uint8& NewLevel);

public:
	// 적용중인 연속처치 버프 레벨이 변경되면 호출됩니다.
	FKillLevelSignature OnKillStreakBuffLevelChanged;

	// 버프가 적용되면 호출됩니다.
	FBuffSignature OnBuffApplied;

	// 버프가 해제되면 호출됩니다.
	FBuffSignature OnBuffExpired;

private:
	UPROPERTY(ReplicatedUsing=OnRep_KillStreakBuffLevel, Transient)
	uint8 KillStreakBuffLevel;
};
