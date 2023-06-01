// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbility.h"
#include "ReloadAbility.generated.h"


DECLARE_EVENT_OneParam(UReloadAbility, FIsReloadingSignature, bool)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UReloadAbility : public UCharacterAbility
{
	GENERATED_BODY()

public:
	UReloadAbility();

	virtual void OnAliveStateChanged(const bool& AliveState) override;

protected:
	virtual void RemoteAbilityStart(const float& RequestTime) override;
	virtual bool ShouldStartRemoteCall() override;

public:
	// 현재 캐릭터가 재장전중인지를 나타냅니다.
	UFUNCTION(BlueprintCallable)
	const bool& IsReloading() const { return bRecentReloadState; }

	// 재장전 완료까지 남은 시간을 반환합니다. 단순히 재장전 완료 예정 시간에 현재 시간을 빼서 반환하므로 음수가 나올 수 있습니다.
	UFUNCTION(BlueprintCallable)
	float GetRemainReloadingTime() const { return ReloadingTime - GetServerTime(); }

protected:
	virtual void ReloadTimerHandler();
	virtual void SetReloadState(const bool& NewState);

	UFUNCTION()
	virtual void OnRep_ReloadingTime();

private:
	void UpdateReloadStateWithTime(const float& CurrentTime);

public:
	// 재장전 상태가 변경되면 호출됩니다.
	FIsReloadingSignature OnReloadStateChanged;

protected:
	// 재장전이 몇초동안 진행될지 지정합니다.
	UPROPERTY(EditAnywhere)
	float ReloadDelay;

private:
	// 재장전이 끝나는 시간을 나타냅니다.
	UPROPERTY(ReplicatedUsing=OnRep_ReloadingTime, Transient)
	float ReloadingTime;

	bool bRecentReloadState;
	FTimerHandle ReloadTimer;
};
