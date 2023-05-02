// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RiffleFireServer.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "DrawDebugHelpers.h"
#include "Character/CharAnimInstance.h"
#include "RiffleFireClient.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class LAKAYA_API URiffleFireClient : public URiffleFireServer
{
	GENERATED_BODY()

protected:
	virtual void OnFireStartNotify() override;
	virtual void OnFireStopNotify() override;
	virtual void OnRep_Character() override;
	virtual void OnCharacterDead() override;

private:
	UFUNCTION(NetMulticast, Unreliable)
	void TraceVisualize();

	FCollisionQueryParams TraceQueryParams;
	FTimerHandle FireTimer;
	EGunSelector Selector;
	uint16 FireCount;
};