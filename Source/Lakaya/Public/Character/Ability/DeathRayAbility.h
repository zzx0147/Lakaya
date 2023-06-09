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
	
public:
	virtual void InitializeComponent() override;
	virtual void LocalAbilityStart() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual bool ShouldStartRemoteCall() override;
	virtual void OnDelayedAbilityStartTimeChanged(const float& NewDelayedAbilityStartTime) override;
	virtual void StartDelayedAbility() override;
	virtual void StopDelayedAbility() override;

public:
	FDeathRayPerformTimeSignature OnDeathRayPerformTimeNotified;
	
private:
	TWeakObjectPtr<class UNiagaraComponent> LaserEffect;
	TWeakObjectPtr<class UArrowComponent> MuzzleComponent;
};
