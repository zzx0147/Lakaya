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

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	void GainPoint(const uint8& GainedPoint);
	void ResetPoint();
	const uint8& GetPoint() const;

private:
	// Client must NOT change this value
	UPROPERTY(Replicated, Transient)
	uint8 Point;
};
