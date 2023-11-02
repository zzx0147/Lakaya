// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/SpringArmComponent.h"
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
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	UBehaviorTreeComponent* BehaviorTreeComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(EditAnywhere)
	bool bIsBehaviorTreeStart;

protected:
	FRotator SmoothTargetRotation;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SmoothFocusInterpSpeed = 50.0f;

private:
	USpringArmComponent* SpringArm;
	FVector AISpringArmOffset;

};
