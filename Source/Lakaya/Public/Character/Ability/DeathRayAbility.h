// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Ability/AutoFireAbility.h"
#include "DeathRayAbility.generated.h"

DECLARE_EVENT_OneParam(UDeathRayAbility, FDeathRayPerformTimeSignature, const float&)

UCLASS()
class LAKAYA_API UDeathRayAbility : public UAutoFireAbility
{
	GENERATED_BODY()

public:
	UDeathRayAbility();

	virtual void InitializeComponent() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual bool ShouldStartRemoteCall() override;
	virtual void StartDelayedAbility() override;
	virtual void OnDelayedAbilityStopTimeChanged(const float& NewDelayedAbilityStopTime) override;

private:
	void DeathRayTimerCallback();

protected:
	// 기본 적용될 능력의 길이입니다.
	UPROPERTY(EditAnywhere)
	float BaseAbilityDuration;

private:
	TWeakObjectPtr<class UNiagaraComponent> LaserEffect;
	FTimerHandle DeathRayTimer;
};
