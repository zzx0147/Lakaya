// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
// 필요없는 헤더
#include "NavigationSystem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Character/ArmedCharacter.h"
#include "Character/InteractableCharacter.h"
// #include "Character/OccupationCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
// 필요없는 헤더
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

	UFUNCTION(BlueprintCallable)
	void AIFireStart(/*class*/AArmedCharacter* ArmCharacter);

	UFUNCTION(BlueprintCallable)
	void AIFireStop(AArmedCharacter* ArmCharacter);

private:
	TWeakObjectPtr<class AArmedCharacter> ArmedCharacter;
   
};