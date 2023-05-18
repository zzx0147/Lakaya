// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AiCharacterController.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AAiCharacterController : public AAIController
{
	GENERATED_BODY()

public:
	AAiCharacterController();

private:
	UFUNCTION(BlueprintCallable)
	void AIFireStart(class AArmedCharacter* ArmCharacter);

	UFUNCTION(BlueprintCallable)
	void AIFireStop(AArmedCharacter* ArmCharacter);

private:
	TWeakObjectPtr<AArmedCharacter> ArmedCharacter;
};
