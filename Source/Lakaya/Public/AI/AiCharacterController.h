// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Character/ArmedCharacter.h"
#include "Character/InteractableCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
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

	UFUNCTION(BlueprintCallable)
	void AiMove(FVector TargetLocation);

	UFUNCTION(BlueprintCallable)
	void AiFireStart(AOccupationCharacter* OccuCharacter);

	UFUNCTION(BlueprintCallable)
	void AiFireStop();

private:
	TWeakObjectPtr<class AArmedCharacter> ArmedCharacter;
	
};
