// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Ability/AutoFireAbility.h"
#include "DeathRayAbility.generated.h"

UCLASS()
class LAKAYA_API UDeathRayAbility : public UAutoFireAbility
{
	GENERATED_BODY()
public:
	UDeathRayAbility();
	
public:
	virtual void InitializeComponent() override;
	virtual void LocalAbilityStart() override;

protected:
	virtual bool ShouldStartRemoteCall() override;
	virtual void RemoteAbilityStart(const float& RequestTime) override;
	virtual void RemoteAbilityStop(const float& RequestTime) override;

private:
	void OnChangeIsFiring(bool bIsFiringArg);

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	TWeakObjectPtr<class UNiagaraComponent> LaserEffect;
	
	UPROPERTY(EditAnywhere)
	float AbilityDuration;

	FTimerHandle AbilityTimer;

	TWeakObjectPtr<class UArrowComponent> MuzzleComponent;
};
