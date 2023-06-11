// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbility.h"
#include "AutoFireAbility.generated.h"

DECLARE_EVENT_OneParam(UAutoFireAbility, FIsFiringSignature, bool)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UAutoFireAbility : public UCharacterAbility
{
	GENERATED_BODY()

public:
	UAutoFireAbility();

	virtual bool ShouldStartRemoteCall() override;
	virtual bool ShouldStopRemoteCall() override;
	virtual void OnAliveStateChanged(const bool& AliveState) override;
	virtual void InitializeComponent() override;

protected:
	virtual void StartDelayedAbility() override;
	virtual void RemoteAbilityStop(const float& RequestTime) override;
	
public:
	UFUNCTION(BlueprintSetter)
	void SetBasisComponent(USceneComponent* NewComponent);

	FORCEINLINE const bool& IsWantsToFire() const { return bWantsToFire; }

protected:
	// 발사 가능 여부를 판별합니다.
	virtual bool ShouldFire();

	// 매 발사마다 호출됩니다.
	virtual void SingleFire();

	// 발사에 실패하는 경우 호출됩니다.
	virtual void FailToFire();

private:
	void FireTick();

protected:
	// 초탄 이후 격발 지연시간을 의미합니다. 이 지연시간에 따라 연사속도가 결정됩니다.
	UPROPERTY(EditAnywhere)
	float FireDelay;

	// 캐릭터 기준 사정거리를 정의합니다.
	UPROPERTY(EditAnywhere)
	float FireRange;

	// 사격의 기본피해량을 정의합니다.
	UPROPERTY(EditAnywhere)
	float FireDamage;

	UPROPERTY(BlueprintSetter=SetBasisComponent)
	TObjectPtr<USceneComponent> BasisComponent;

	FCollisionQueryParams CollisionQueryParams;

private:
	bool bWantsToFire;
	FTimerHandle FireTimer;
};
