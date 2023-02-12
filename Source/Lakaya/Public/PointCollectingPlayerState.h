// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InGamePlayerState.h"
#include "PointCollectingPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API APointCollectingPlayerState : public AInGamePlayerState
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	uint8 Point;

public:
	void GainPoint(const uint8& GainedPoint);
	void ResetPoint();
	const uint8& GetPoint() const;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
