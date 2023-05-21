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

	virtual void LocalAbilityStart() override;
	virtual void OnAliveStateChanged(const bool& AliveState) override;

protected:
	virtual void BeginPlay() override;
	virtual void RemoteAbilityStart(const float& RequestTime) override;

	UFUNCTION()
	virtual void OnRep_IsReloading();

	virtual void ReloadTimerHandler();

public:
	// 재장전 상태가 변경되면 호출됩니다.
	FIsReloadingSignature OnReloadStateChanged;

protected:
	UPROPERTY(EditAnywhere)
	float ReloadDelay;

private:
	UPROPERTY(ReplicatedUsing=OnRep_IsReloading, Transient)
	bool bIsReloading;

	FTimerHandle OwnerTimer;
	TWeakObjectPtr<class UBulletComponent> BulletComponent;
};
