// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AiCharacterController.h"

#include "Character/ArmedCharacter.h"

AAiCharacterController::AAiCharacterController() // 생성자
{
	bWantsPlayerState = true;

	// 블랙보드와 비헤이비어 트리 컴포넌트 생성
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));

}

void AAiCharacterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// OnPossess 되었을때 비헤이비어 트리 에셋 할당해주면 컴포넌트에 있는 블랙보드와 비헤이비어 트리를 시작시켜줌
	if (BehaviorTreeAsset)
	{
		BlackboardComp->InitializeBlackboard(*(BehaviorTreeAsset->BlackboardAsset));
		BehaviorTreeComp->StartTree(*(BehaviorTreeAsset));
	}
}

void AAiCharacterController::AIFireStart(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StartAbility(WeaponFire);
}

void AAiCharacterController::AIFireStop(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StopAbility(WeaponFire);
}
