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

	virtual void LocalAbilityStart() override;
	virtual void LocalAbilityStop() override;

protected:
	virtual void BeginPlay() override;
	virtual void RemoteAbilityStart(const float& RequestTime) override;
	virtual void RemoteAbilityStop(const float& RequestTime) override;
	
	UFUNCTION()
	virtual void OnRep_AbilityStartTime();
	
public:
	UFUNCTION(BlueprintGetter)
	const bool& IsFiring() const { return bIsFiring; }

protected:
	UFUNCTION()
	virtual void OnRep_IsFiring();

	// 발사 가능 여부를 판별합니다.
	virtual bool ShouldFire();

	// 매 발사마다 호출됩니다.
	virtual void SingleFire();

	// 발사에 실패하는 경우 호출됩니다.
	virtual void FailToFire();

private:
	void FireTick();

public:
	// 사격상태가 변경되면 호출됩니다. true이면 사격중, 그렇지 않으면 사격중지를 의미합니다.
	FIsFiringSignature OnFiringStateChanged;

protected:
	// 초탄 발사 지연시간을 의미합니다.
	UPROPERTY(EditAnywhere)
	float InitDelay;

	// 초탄 이후 격발 지연시간을 의미합니다. 이 지연시간에 따라 연사속도가 결정됩니다.
	UPROPERTY(EditAnywhere)
	float FireDelay;

	// 캐릭터 기준 사정거리를 정의합니다.
	UPROPERTY(EditAnywhere)
	float FireRange;

	// 사격의 기본피해량을 정의합니다.
	UPROPERTY(EditAnywhere)
	float FireDamage;

	//사격 요청이 오고 실제로 사격이 시작되는 시간입니다. 음수일 경우 사격 중지를 의미합니다
	UPROPERTY(ReplicatedUsing = OnRep_AbilityStartTime, Transient)
	float AbilityStartTime;
	
protected:
	FCollisionQueryParams CollisionQueryParams;
	
private:
	UPROPERTY(ReplicatedUsing=OnRep_IsFiring, Transient)
	bool bIsFiring;

	bool bIsFireRequested;
	FTimerHandle FireTimer;
	TWeakObjectPtr<class UCameraComponent> Camera;
	TWeakObjectPtr<USceneComponent> RootComponent;

};
