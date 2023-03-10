// Fill out your copyright notice in the Description page of Project Settings.


#include "ZoomAbility.h"

#include "ThirdPersonCharacter.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"

UZoomAbility::UZoomAbility()
{
	FOVMultiplier = 0.5f;
}

void UZoomAbility::AbilityStart()
{
	if (Camera.IsValid()) Camera->SetFieldOfView(Camera->FieldOfView * FOVMultiplier);
	else UE_LOG(LogNetSubObject, Warning, TEXT("Camera is Invalid!"));
}

void UZoomAbility::AbilityStop()
{
	if (Camera.IsValid()) Camera->SetFieldOfView(Camera->FieldOfView / FOVMultiplier);
	else UE_LOG(LogNetSubObject, Warning, TEXT("Camera is Invalid!"));
}

void UZoomAbility::SetupData_Implementation(const FName& RowName)
{
	Super::SetupData_Implementation(RowName);
	
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

	DOREPLIFETIME(UZoomAbility, Camera);
}
