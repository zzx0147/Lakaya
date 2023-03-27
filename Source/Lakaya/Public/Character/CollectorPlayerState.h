// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CollectorPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API ACollectorPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ACollectorPlayerState();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void GainPoint(const uint8& GainedPoint);
	void ResetPoint();
	const uint8& GetPoint() const;

	void GainEnergy(const uint8& GainedEnergy);
	void ResetEnergy();
	const uint8& GetEnergy() const;

	void GainMoney(const uint8& GainedMoney);
	void ResetMoney();
	const uint8& GetMoney() const;

private:
	UPROPERTY(ReplicatedUsing = OnRep_Point, Transient)
	uint8 Point;

	UPROPERTY(ReplicatedUsing = OnRep_Money, Transient)
	uint8 Money;
	
	UPROPERTY(ReplicatedUsing = OnRep_Energy, Transient)
	uint8 Energy;

private:
	UFUNCTION()
	void OnRep_Point();
	
	UFUNCTION()
	void OnRep_Money();
	
	UFUNCTION()
	void OnRep_Energy();
};