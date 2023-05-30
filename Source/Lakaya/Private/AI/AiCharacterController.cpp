// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AiCharacterController.h"

#include "Character/ArmedCharacter.h"

AAiCharacterController::AAiCharacterController() // 생성자
{
	bWantsPlayerState = true;

	// 블랙보드와 비헤이비어 트리 컴포넌트 생성
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));

	// AI의 SpringArm SocketOffset 설정을 위한 변수
	AISpringArmOffset = FVector(0,0,-54);
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

	// 카메라와 총구의 사잇값에서 총알이나가는 문제때문에 AI가 총알을 휘게쏴서 0으로 맞춰줌!
	SpringArm = GetPawn()->FindComponentByClass<USpringArmComponent>();
	if (SpringArm)
		SpringArm->SocketOffset = AISpringArmOffset;
}

void AAiCharacterController::AIFireStart(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StartAbility(WeaponFire);
}

void AAiCharacterController::AIFireStop(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StopAbility(WeaponFire);
}
