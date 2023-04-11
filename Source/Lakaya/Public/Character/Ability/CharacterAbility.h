// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAbility.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UCharacterAbility : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void AbilityStart();
	virtual void AbilityStop();

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestStart(const float& RequestTime);

	UFUNCTION(Server, Reliable, WithValidation)
	void RequestStop(const float& RequestTime);
};
