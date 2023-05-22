// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraComponent.h"
#include "Character/BulletComponent.h"
#include "Character/ResourceComponent.h"
#include "Character/Ability/CharacterAbility.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

UCharacterAbility::UCharacterAbility()
{
	bWantsInitializeComponent = true;
}

void UCharacterAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterAbility, EnableTime);
}

void UCharacterAbility::InitializeComponent()
{
	Super::InitializeComponent();
	CameraComponent = GetOwner()->FindComponentByClass<UCameraComponent>();
	ResourceComponent = GetOwner()->FindComponentByClass<UResourceComponent>();
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

bool UCharacterAbility::CostResource(const EResourceKind& ResourceKind, const float& Value)
{
	switch (ResourceKind)
	{
	case EResourceKind::Bullet:
		{
			if (const auto BulletComponent = GetResourceComponent<UBulletComponent>())
				return BulletComponent->CostBullet(Value);
			return false;
		}
	default:
		return false;
	}
}

bool UCharacterAbility::CostResource(const TArray<FResourceCostData>& CostArray)
{
	// 자원들이 모두 소모가능한 만큼 충분히 존재하는지 체크합니다.
	for (const auto& [ResourceKind, Value] : CostArray)
	{
		switch (ResourceKind)
		{
		case EResourceKind::Bullet:
			{
				if (const auto BulletComponent = GetResourceComponent<UBulletComponent>();
					!BulletComponent || !BulletComponent->IsEnough(Value))
					return false;
				break;
			}
		default:
			return false;
		}
	}

	// 실제로 자원들에 대한 소모를 진행합니다.
	for (const auto& [ResourceKind, Value] : CostArray)
	{
		switch (ResourceKind)
		{
		case EResourceKind::Bullet:
			GetResourceComponent<UBulletComponent>()->CostBullet(Value);
			break;
		}
	}
	return true;
}

void UCharacterAbility::OnRep_EnableTime()
{
	OnEnableTimeChanged.Broadcast(EnableTime);
}

void UCharacterAbility::ApplyCoolTime()
{
	EnableTime = GetServerTime() + GetCoolTime();
	OnEnableTimeChanged.Broadcast(EnableTime);
}
