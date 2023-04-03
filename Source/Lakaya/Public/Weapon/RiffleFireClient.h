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

protected:
	virtual void OnFireStartNotify() override;
	virtual void OnFireStopNotify() override;
	virtual void OnSwitchSelectorNotify() override;
	virtual void OnRep_Character() override;

private:
	void TraceVisualize();

	FCollisionQueryParams TraceQueryParams;
	FTimerHandle FireTimer;
	FTimerHandle SelectorTimer;
	EGunSelector Selector;
	EGunSelector DesiredSelector;
	uint16 FireCount;
};
