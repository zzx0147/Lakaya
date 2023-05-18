// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraComponent.h"
#include "Character/Ability/CharacterAbility.h"
#include "GameFramework/GameStateBase.h"

UCharacterAbility::UCharacterAbility()
{
	bWantsInitializeComponent = true;
}

void UCharacterAbility::InitializeComponent()
{
	Super::InitializeComponent();
	CameraComponent = GetOwner()->FindComponentByClass<UCameraComponent>();
	bRecentAliveState = true;
}

void UCharacterAbility::OnAliveStateChanged(const bool& AliveState)
{
	bRecentAliveState = AliveState;
}

bool UCharacterAbility::ShouldStartRemoteCall()
{
	return false;
}

bool UCharacterAbility::ShouldStopRemoteCall()
{
	return false;
}

bool UCharacterAbility::CanStartRemoteCall()
{
	return bCanEverStartRemoteCall && ShouldStartRemoteCall();
}

bool UCharacterAbility::CanStopRemoteCall()
{
	return bCanEverStopRemoteCall && ShouldStopRemoteCall();
}

FVector UCharacterAbility::GetDistancedToCameraDirection(const float& Distance, const float& FromActor,
                                                         const FCollisionQueryParams& CollisionParams) const
{
	if (!CameraComponent.IsValid()) return FVector::ZeroVector;
	return GetOwner()->GetActorLocation() + GetNormalToCameraForwardTracePoint(FromActor, CollisionParams) * Distance;
}

FVector UCharacterAbility::GetCameraForwardTracePoint(const float& FromActor,
                                                      const FCollisionQueryParams& CollisionQueryParams) const
{
	if (!CameraComponent.IsValid()) return FVector::ZeroVector;

	FHitResult Result;
	return GetWorld()->LineTraceSingleByChannel(Result, CameraComponent->GetComponentLocation(),
	                                            GetCameraForwardPointFromActor(FromActor),
	                                            ECC_Visibility, CollisionQueryParams)
		       ? Result.ImpactPoint
		       : Result.TraceEnd;
}

FVector UCharacterAbility::GetCameraForwardPointFromActor(const float& FromActor) const
{
	if (!CameraComponent.IsValid()) return FVector::ZeroVector;

	const auto CameraLocation = CameraComponent->GetComponentLocation();
	const auto CameraForward = CameraComponent->GetForwardVector();

	// 카메라에서 액터까지의 이동벡터를 카메라의 전방벡터에 투영한 값에 FromActor를 더하여 항상 액터로부터 FromActor만큼 떨어진 곳임을 보장받습니다.
	return CameraLocation + (CameraForward.Dot(GetOwner()->GetActorLocation() - CameraLocation) + FromActor) *
		CameraForward;
}

FVector UCharacterAbility::GetNormalToCameraForwardTracePoint(const float& FromActor,
                                                              const FCollisionQueryParams& CollisionQueryParams) const
{
	return (GetCameraForwardTracePoint(FromActor, CollisionQueryParams) - GetOwner()->GetActorLocation()).
		GetUnsafeNormal();
}

float UCharacterAbility::GetServerTime() const
{
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}
