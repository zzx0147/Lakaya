// Fill out your copyright notice in the Description page of Project Settings.


#include "ETC/LakayaPlayerCameraManager.h"

#include "Character/LakayaBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void ALakayaPlayerCameraManager::Zoom_Implementation(const bool IsZoomIn, const float& ZoomFov)
{
	K2_SetFov(IsZoomIn ? ZoomFov : DefaultFOV);
}

void ALakayaPlayerCameraManager::K2_SetFov(const float& NewFov)
{
	SetFOV(NewFov);
}

void ALakayaPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	if (const ALakayaBaseCharacter* BaseCharacter = Cast<ALakayaBaseCharacter>(GetOwningPlayerController()->GetPawn()))
	{
		const UCharacterMovementComponent* CharacterMovementComponent = BaseCharacter->GetCharacterMovement();
		const FVector TargetCrouchOffset = FVector(
			0,
			0,
			CharacterMovementComponent->GetCrouchedHalfHeight() - BaseCharacter->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
		);
		FVector Offset = FMath::Lerp(FVector::ZeroVector, TargetCrouchOffset, FMath::Clamp(CrouchBlendTime / CrouchBlendDuration, 0.f, 1.f));

		if (CharacterMovementComponent->IsCrouching())
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime + DeltaTime, 0.f, CrouchBlendDuration);
			Offset -= TargetCrouchOffset;
		}
		else
		{
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime - DeltaTime, 0.f, CrouchBlendDuration);
		}

		OutVT.POV.Location += Offset;
	}
}
