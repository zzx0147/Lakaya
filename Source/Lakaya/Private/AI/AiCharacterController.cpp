// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AiCharacterController.h"

#include "Character/ArmedCharacter.h"
#include "Kismet/KismetMathLibrary.h"

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
	if (bIsBehaviorTreeStart == true && BehaviorTreeAsset)
	{
		BlackboardComp->InitializeBlackboard(*(BehaviorTreeAsset->BlackboardAsset));
		BehaviorTreeComp->StartTree(*(BehaviorTreeAsset));
	}

	// 카메라와 총구의 사잇값에서 총알이나가는 문제때문에 AI가 총알을 휘게쏴서 0으로 맞춰줌!
	SpringArm = GetPawn()->FindComponentByClass<USpringArmComponent>();
	if (SpringArm)
		SpringArm->SocketOffset = AISpringArmOffset;
}

void AAiCharacterController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
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

void AAiCharacterController::AIFireStart(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StartAbility(WeaponFire);
}

void AAiCharacterController::AIFireStop(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StopAbility(WeaponFire);
}

void AAiCharacterController::AIReloadStart(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StartAbility(WeaponReload);
}

void AAiCharacterController::AIReloadStop(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StopAbility(WeaponReload);
}

void AAiCharacterController::AIRemainBulletCheck(AArmedCharacter* ArmCharacter, uint8& RemainBullet)
{
	// BulletComponent = GetPawn()->FindComponentByClass<UBulletComponent>();
	//
	// if (ArmCharacter)
	// {
	// 	RemainBullet = BulletComponent->GetBullets();
	// }
}

void AAiCharacterController::AIPrimarySkillStart(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StartAbility(Primary);
}

void AAiCharacterController::AISecondarySkillStart(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StartAbility(Secondary);
}

void AAiCharacterController::AIWeaponSkillStart(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StartAbility(WeaponAbility);
}

void AAiCharacterController::AIPrimarySkillStop(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StopAbility(Primary);
}

void AAiCharacterController::AISecondarySkillStop(AArmedCharacter* ArmCharacter)
{
	if (ArmCharacter) ArmCharacter->StopAbility(Secondary);
}
