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

	virtual void InitializeComponent() override;
	virtual void OnAliveStateChanged(const bool& AliveState) override;

protected:
	virtual void RemoteAbilityStart(const float& RequestTime) override;
	virtual bool ShouldStartRemoteCall() override;

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
	TWeakObjectPtr<class UBulletComponent> BulletComponent;
};
