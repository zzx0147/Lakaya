// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
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

	virtual void OnPossess(APawn* InPawn) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	UBehaviorTreeComponent* BehaviorTreeComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTreeAsset;
	
private:
	UFUNCTION(BlueprintCallable)
	void AIFireStart(class AArmedCharacter* ArmCharacter);

	UFUNCTION(BlueprintCallable)
	void AIFireStop(AArmedCharacter* ArmCharacter);

private:
	TWeakObjectPtr<AArmedCharacter> ArmedCharacter;
};
