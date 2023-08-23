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

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	UBehaviorTreeComponent* BehaviorTreeComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTreeAsset;

	UPROPERTY(EditAnywhere)
	bool bIsBehaviorTreeStart;
	
private:
	//TODO: 매개변수로 EAbilityKind를 넘기는 편이 어떨까..
	UFUNCTION(BlueprintCallable)
	void AIFireStart(class AArmedCharacter* ArmCharacter);

	UFUNCTION(BlueprintCallable)
	void AIFireStop(AArmedCharacter* ArmCharacter);

	UFUNCTION(BlueprintCallable)
	void AIReloadStart(AArmedCharacter* ArmCharacter);
	
	UFUNCTION(BlueprintCallable)
	void AIReloadStop(AArmedCharacter* ArmCharacter);
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	void AIRemainBulletCheck(AArmedCharacter* ArmCharacter, uint8& RemainBullet);

	UFUNCTION(BlueprintCallable, Category = "AI Skill")
	void AIPrimarySkillStart(AArmedCharacter* ArmCharacter);

	UFUNCTION(BlueprintCallable, Category = "AI Skill")
	void AISecondarySkillStart(AArmedCharacter* ArmCharacter);
	
	UFUNCTION(BlueprintCallable, Category = "AI Skill")
    void AIWeaponSkillStart(AArmedCharacter* ArmCharacter);
	
	UFUNCTION(BlueprintCallable, Category = "AI Skill")
	void AIPrimarySkillStop(AArmedCharacter* ArmCharacter);

	UFUNCTION(BlueprintCallable, Category = "AI Skill")
	void AISecondarySkillStop(AArmedCharacter* ArmCharacter);

private:
	//TODO: 사용되지 않음
	TWeakObjectPtr<AArmedCharacter> ArmedCharacter;

	TWeakObjectPtr<class UBulletComponent> BulletComponent;
	USpringArmComponent* SpringArm;
	FVector AISpringArmOffset;

};
