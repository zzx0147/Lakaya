// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ZoomAbility.h"

#include "Character/ThirdPersonCharacter.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

void UZoomAbility::AbilityStart()
{
	if (Camera.IsValid())
	{
		if (!bIsZoomed)
		{
			Camera->SetFieldOfView(Camera->FieldOfView * FOVMultiplier);
			bIsZoomed = true;
		}
	}
	else UE_LOG(LogNetSubObject, Warning, TEXT("Camera is Invalid!"));
}

void UZoomAbility::AbilityStop()
{
	if (Camera.IsValid())
	{
		if (bIsZoomed)
		{
			Camera->SetFieldOfView(Camera->FieldOfView / FOVMultiplier);
			bIsZoomed = false;
		}
	}
	else UE_LOG(LogNetSubObject, Warning, TEXT("Camera is Invalid!"));
}

void UZoomAbility::OnCharacterDead()
{
	Super::OnCharacterDead();
	AbilityStop();
}

void UZoomAbility::SetupData(const FName& RowName)
{
	Super::SetupData(RowName);

	FOVMultiplier = 0.5f;
	if (auto Outer = Cast<UActorComponent>(GetOuter()))
		if (auto Actor = Cast<AThirdPersonCharacter>(Outer->GetOwner()))
		{
			Camera = Actor->GetCamera();
			return;
		}
	UE_LOG(LogNetSubObject, Warning, TEXT("Something's wrong whith setup UZoomAbility"));
}

void UZoomAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UZoomAbility, Camera, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UZoomAbility, FOVMultiplier, COND_OwnerOnly);
}
