// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RiffleFireClient.h"
#include "RiffleFireOwner.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API URiffleFireOwner : public URiffleFireClient
{
	GENERATED_BODY()

public:
	virtual void FireStart() override;
	virtual void FireStop() override;
	virtual void OnCharacterDead() override;

private:
	FTimerHandle FireTimer;
	FTimerHandle SelectorTimer;
	EGunSelector Selector;
	uint16 FireCount;
};
