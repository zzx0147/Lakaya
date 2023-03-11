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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void GainPoint(const uint8& GainedPoint);
	void ResetPoint();
	const uint8& GetPoint() const;

	void GainEnergy(const uint8& GainedEnergy);
	void ResetEnergy();
	const uint8& GetEnergy() const;

	UFUNCTION()
	void OnRep_Energy();
private:
	 // Client must NOT change this value
	 UPROPERTY(Replicated, Transient)
	 uint8 Point;

	UPROPERTY(ReplicatedUsing = OnRep_Energy, Transient)
	uint8 Energy;
};