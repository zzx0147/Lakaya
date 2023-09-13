// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AiDroneController.h"

AAiDroneController::AAiDroneController()
{
	bWantsPlayerState = true;

	// 블랙보드와 비헤이비어 트리 컴포넌트 생성
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComp"));
}

void AAiDroneController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// OnPossess 되었을때 비헤이비어 트리 에셋 할당해주면 컴포넌트에 있는 블랙보드와 비헤이비어 트리를 시작시켜줌
	if (bIsBehaviorTreeStart == true && BehaviorTreeAsset)
	{
		BlackboardComp->InitializeBlackboard(*(BehaviorTreeAsset->BlackboardAsset));
		BehaviorTreeComp->StartTree(*(BehaviorTreeAsset));
	}
}
