// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StartRemoteCallAbility.h"
#include "StopRemoteCallAbility.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UStopRemoteCallAbility : public UStartRemoteCallAbility
{
	GENERATED_BODY()

public:
	virtual void AbilityStop() override;

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestStop(const float& RequestTime);
	virtual bool RequestStop_Validate(const float& RequestTime);
	virtual void RequestStop_Implementation(const float& RequestTime) { return; }
};
