// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbility.h"
#include "StartRemoteCallAbility.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UStartRemoteCallAbility : public UCharacterAbility
{
	GENERATED_BODY()

public:
	virtual void AbilityStart() override;

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestStart(const float& RequestTime);
	virtual bool RequestStart_Validate(const float& RequestTime);
	virtual void RequestStart_Implementation(const float& RequestTime) {return;}

	float GetServerTime() const;
};
