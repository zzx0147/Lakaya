// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AiDroneController.h"
#include "Kismet/KismetMathLibrary.h"

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

void AAiDroneController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	Super::UpdateControlRotation(DeltaTime, false);

	//AI 의 적군 포커싱을 부드럽게 회전하게끔 하는 함수입니당.
	if (bUpdatePawn)
	{
		APawn* const MyPawn = GetPawn();
		if(MyPawn)
		{
			const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();
			SmoothTargetRotation = UKismetMathLibrary::RInterpTo_Constant(MyPawn->GetActorRotation(), ControlRotation, DeltaTime, SmoothFocusInterpSpeed);
		
			if (CurrentPawnRotation.Equals(SmoothTargetRotation, 1e-3f) == false)
			{
				MyPawn->FaceRotation(SmoothTargetRotation, DeltaTime);
			}
		}
	}
}
