// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RiffleFireServer.h"
#include "Character/CharAnimInstance.h"
#include "RiffleFireClient.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class LAKAYA_API URiffleFireClient : public URiffleFireServer
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void SetIsGPSFire(bool bIsGPSFire);

protected:
	virtual void OnFireStartNotify() override;
	virtual void OnFireStopNotify() override;
	virtual void OnRep_Character() override;

private:
	void TraceVisualize();

	FCollisionQueryParams TraceQueryParams;
	FTimerHandle FireTimer;
	EGunSelector Selector;
	uint16 FireCount;
};
