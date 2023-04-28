// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LakayaBasePlayerState.h"
#include "StatPlayerState.generated.h"

UCLASS()
class LAKAYA_API AStatPlayerState : public ALakayaBasePlayerState
{
	GENERATED_BODY()

public:
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	virtual float GetMaxHealth() const override;

protected:
	virtual float InternalTakePointDamage(float Damage, FPointDamageEvent const& PointDamageEvent,
	                                      AController* EventInstigator, AActor* DamageCauser) override;
	virtual float InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent,
	                                       AController* EventInstigator, AActor* DamageCauser) override;

	/**
	 * @brief 현재 스탯에 따라 경감된 데미지를 반환합니다.
	 * @param Damage 원본 피해량입니다.
	 * @return 경감된 피해량입니다.
	 */
	float ApplyReduceStatToDamage(const float& Damage) const;
};
