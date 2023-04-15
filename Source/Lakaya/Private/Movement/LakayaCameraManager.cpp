// Fill out your copyright notice in the Description page of Project Settings.


#include "Movement/LakayaCharacterMovementComponent.h"
#include "Character/LakayaMovementTestCharacter.h"
#include "Movement/LakayaCameraManager.h"
#include "Components/CapsuleComponent.h"

ALakayaCameraManager::ALakayaCameraManager()
{
}

void ALakayaCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	if (ALakayaMovementTestCharacter* LakayaCharacter = Cast<ALakayaMovementTestCharacter>(GetOwningPlayerController()->GetPawn()))
	{
		ULakayaCharacterMovementComponent* LMC = LakayaCharacter->GetLakayaCharacterMovement();
		FVector TargetCrouchOffset = FVector(
			0, 0, LMC->GetCrouchedHalfHeight() - LakayaCharacter->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		
		FVector Offset = FMath::Lerp(FVector::ZeroVector, TargetCrouchOffset, FMath::Clamp(CrouchBlendTime / CrouchBlendDuration, 0.0f, 1.0f));

		if (LMC->IsCrouching())
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime + DeltaTime, 0.0f, CrouchBlendDuration);
			Offset -= TargetCrouchOffset;
		}
		else
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime + DeltaTime, 0.0f, CrouchBlendDuration);
		}

		if (LMC->IsMovingOnGround())
		{
			OutVT.POV.Location += Offset;
		}
	}
}
