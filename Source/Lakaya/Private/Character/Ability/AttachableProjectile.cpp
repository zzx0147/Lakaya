// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/AttachableProjectile.h"

#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

void AAttachableProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAttachableProjectile, ActivationTime);
	DOREPLIFETIME(AAttachableProjectile, TargetActor);
	DOREPLIFETIME(AAttachableProjectile, RelativeLocation);
	DOREPLIFETIME(AAttachableProjectile, RelativeRotator);
	DOREPLIFETIME(AAttachableProjectile, AttachedBone);
}

AAttachableProjectile::AAttachableProjectile()
{
	bReplicates = true;
	ActivationDelay = 0.5f;
}

float AAttachableProjectile::GetServerTime() const
{
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void AAttachableProjectile::SureActivation(const float& Delay)
{
	Delay > 0.f
		? GetWorld()->GetTimerManager().SetTimer(ActivationTimer, this, &AAttachableProjectile::OnActivation, Delay)
		: OnActivation();
}

void AAttachableProjectile::InitializeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                              const FHitResult& SweepResult, const FVector& Velocity)
{
	TargetActor = OtherActor;
	// 투사체가 진행중에 충돌한 경우 간단히 SweepResult를 사용합니다.
	if (bFromSweep)
	{
		AttachedBone = SweepResult.BoneName;
		SetActorLocationAndRotation(SweepResult.ImpactPoint, SweepResult.ImpactNormal.Rotation());
	}
	else
	{
		// SweepResult를 사용할 수 없는 경우 원래 투사체가 진행하던 방향으로 조금 더 진행시켜서 부착될 위치를 찾습니다.
		const auto Start = OverlappedComponent->GetComponentLocation();
		if (FHitResult HitResult;
			OtherComp->SweepComponent(HitResult, Start, Start + Velocity * 100.f,
			                          OverlappedComponent->GetComponentRotation().Quaternion(),
			                          OverlappedComponent->GetCollisionShape(), true))
		{
			AttachedBone = HitResult.BoneName;
			SetActorLocationAndRotation(HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
		}
		// 충돌에 실패한 경우 그냥 투사체가 최초로 충돌했던 위치로 두고, 진행하던 방향의 반대 방향을 바라보도록 셋팅합니다.
		else
		{
			AttachedBone = NAME_None;
			SetActorLocationAndRotation(Start, (-Velocity).Rotation());
		}
	}
	AttachToActor(TargetActor, FAttachmentTransformRules::KeepWorldTransform, AttachedBone);
	RelativeLocation = GetRootComponent()->GetRelativeLocation();
	RelativeRotator = GetRootComponent()->GetRelativeRotation();

	ActivationTime = GetServerTime() + ActivationDelay;
	SureActivation(ActivationDelay);
}

void AAttachableProjectile::OnRep_ActivationTime()
{
	if (ActivationTime > 0.f)
	{
		AttachToActor(TargetActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachedBone);
		SetActorRelativeLocation(RelativeLocation);
		SetActorRelativeRotation(RelativeRotator);
		SureActivation(ActivationTime - GetServerTime());
	}
	else OnDeactivation();
}

void AAttachableProjectile::OnActivation()
{
}

void AAttachableProjectile::OnDeactivation()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void AAttachableProjectile::Deactivate()
{
	ActivationTime = 0.f;
	OnDeactivation();
}
