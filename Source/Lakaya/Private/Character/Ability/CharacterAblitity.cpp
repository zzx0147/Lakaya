// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraComponent.h"
#include "Character/BulletComponent.h"
#include "Character/ResourceComponent.h"
#include "Character/Ability/CharacterAbility.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

UCharacterAbility::UCharacterAbility()
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
	AbilityStartDelay = AbilityStopDelay = 0.f;
	OnAbilityStartTimeNotified.AddUObject(this, &UCharacterAbility::OnDelayedAbilityStartTimeChanged);
	OnAbilityStopTimeNotified.AddUObject(this, &UCharacterAbility::OnDelayedAbilityStopTimeChanged);
}

void UCharacterAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterAbility, EnableTime);
	DOREPLIFETIME(UCharacterAbility, DelayedAbilityStartTime);
	DOREPLIFETIME(UCharacterAbility, DelayedAbilityStopTime);

	if (AbilityStartDelay > 0.f) DISABLE_REPLICATED_PROPERTY(UCharacterAbility, DelayedAbilityStartTime);
	if (AbilityStopDelay > 0.f) DISABLE_REPLICATED_PROPERTY(UCharacterAbility, DelayedAbilityStopTime);
}

void UCharacterAbility::InitializeComponent()
{
	Super::InitializeComponent();
	CameraComponent = GetOwner()->FindComponentByClass<UCameraComponent>();
	ResourceComponent = GetOwner()->FindComponentByClass<UResourceComponent>();
	bRecentAliveState = true;
}

void UCharacterAbility::RemoteAbilityStart(const float& RequestTime)
{
	if (AbilityStartDelay > 0.f && bUpdateStartTimeOnStart) UpdateAbilityStartTime(RequestTime);
}

void UCharacterAbility::RemoteAbilityStop(const float& RequestTime)
{
	if (AbilityStopDelay > 0.f && bUpdateStopTimeOnStop) UpdateAbilityStopTime(RequestTime);
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
                                                              const FCollisionQueryParams& CollisionQueryParams,
                                                              const USceneComponent* BasisComponent) const
{
	const auto BasisLocation = BasisComponent ? BasisComponent->GetComponentLocation() : GetOwner()->GetActorLocation();
	return (GetCameraForwardTracePoint(FromActor, CollisionQueryParams) - BasisLocation).GetUnsafeNormal();
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

void UCharacterAbility::OnRep_DelayedAbilityStartTime()
{
	OnAbilityStartTimeNotified.Broadcast(DelayedAbilityStartTime);
}

void UCharacterAbility::OnRep_DelayedAbilityStopTime()
{
	OnAbilityStopTimeNotified.Broadcast(DelayedAbilityStopTime);
}

void UCharacterAbility::ApplyCoolTime()
{
	EnableTime = GetServerTime() + GetCoolTime();
	OnEnableTimeChanged.Broadcast(EnableTime);
}

void UCharacterAbility::OnDelayedAbilityStartTimeChanged(const float& NewDelayedAbilityStartTime)
{
	// 아직 능력 시작 시간이 도래하지 않았다면 타이머를 건다.
	if (const auto RemainTime = NewDelayedAbilityStartTime - GetServerTime(); RemainTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(AbilityStartTimerHandle, this, &UCharacterAbility::StartDelayedAbility,
		                                       RemainTime);
	}
	//이미 능력이 시작됐다면 바로 시작한다.
	else StartDelayedAbility();
}

void UCharacterAbility::OnDelayedAbilityStopTimeChanged(const float& NewDelayedAbilityStopTime)
{
	if (const auto RemainTime = NewDelayedAbilityStopTime - GetServerTime(); RemainTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(AbilityStopTimerHandle, this, &UCharacterAbility::StopDelayedAbility,
		                                       RemainTime);
	}
	else StopDelayedAbility();
}

void UCharacterAbility::UpdateAbilityStartTime(const float& Time)
{
	DelayedAbilityStartTime = Time + AbilityStartDelay;
	OnAbilityStartTimeNotified.Broadcast(DelayedAbilityStartTime);
}

void UCharacterAbility::UpdateAbilityStopTime(const float& Time)
{
	DelayedAbilityStopTime = Time + AbilityStopDelay;
	OnAbilityStopTimeNotified.Broadcast(DelayedAbilityStopTime);
}

void UCharacterAbility::StartDelayedAbility()
{
}

void UCharacterAbility::StopDelayedAbility()
{
}
