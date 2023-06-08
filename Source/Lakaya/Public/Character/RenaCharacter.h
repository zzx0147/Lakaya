// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableCharacter.h"
#include "RenaCharacter.generated.h"

UCLASS()
class LAKAYA_API ARenaCharacter : public AInteractableCharacter
{
	GENERATED_BODY()

public:
	explicit ARenaCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void SetAliveState_Implementation(bool IsAlive) override;

protected:
	virtual bool ShouldStartAbility_Implementation(EAbilityKind Kind) override;
	virtual bool ShouldStartAbilityOnServer_Implementation(EAbilityKind Kind) override;

	virtual void OnMinePerformTimeNotified(const float& Time);
	virtual void OnDeathRayStartTimeNotified(const float& Time);
	virtual void OnWantsToFireChanged(bool FireState);
	virtual void OnBombPerformTimeNotified(const float& Time);
	virtual void OnReloadCompleteTimeNotified(const float& Time);
	virtual void OnReloadStateChanged(bool ReloadState);

	// 지뢰를 던지고 나서 몇초동안 후딜레이가 적용되도록 할지 지정합니다.
	UPROPERTY(EditAnywhere)
	float MineLateDelay;

	// 죽음의 광선 스킬이 시작되고 몇초뒤부터 다른 행동이 가능하도록 할지 지정합니다.
	UPROPERTY(EditAnywhere)
	float DeathRayLateDelay;

	// 에너지 폭탄을 던지고 나서 몇초동안 후딜레이가 적용되도록 할지 지정합니다.
	UPROPERTY(EditAnywhere)
	float BombLateDelay;

private:
	float MineEndingTime;
	float DeathRayEndingTime;
	bool bWantsToFire;
	float BombEndingTime;
	float ReloadCompleteTime;
	bool bIsReloading;
};
