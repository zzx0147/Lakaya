// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Character/ArmedCharacter.h"
#include "Character/InteractableCharacter.h"
// #include "Character/OccupationCharacter.h"
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

private:
	virtual void BeginPlay() override;

	// UFUNCTION(BlueprintCallable)
	// void AiFireStart(AOccupationCharacter* OccuCharacter);
	//
	// UFUNCTION(BlueprintCallable)
	// void AiFireStop(AOccupationCharacter* OccuCharacter);

private:
	TWeakObjectPtr<class AArmedCharacter> ArmedCharacter;
   
};