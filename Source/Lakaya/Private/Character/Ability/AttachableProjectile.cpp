// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/AttachableProjectile.h"

#include "Net/UnrealNetwork.h"

void AAttachableProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAttachableProjectile, bIsEnabled);
	DOREPLIFETIME(AAttachableProjectile, TargetActor);
	DOREPLIFETIME(AAttachableProjectile, RelativeLocation);
	DOREPLIFETIME(AAttachableProjectile, RelativeRotator);
	DOREPLIFETIME(AAttachableProjectile, AttachedBone);
}

AAttachableProjectile::AAttachableProjectile()
{
	bReplicates = true;
}

void AAttachableProjectile::InitializeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                              const FHitResult& SweepResult, const FVector& Velocity)
{
	bIsEnabled = true;
	TargetActor = OtherActor;
	if (bFromSweep)
	{
		AttachedBone = SweepResult.BoneName;
		SetActorLocationAndRotation(SweepResult.ImpactPoint, SweepResult.ImpactNormal.Rotation());
	}
	else
	{
		const auto Start = OverlappedComponent->GetComponentLocation();
		if (FHitResult HitResult;
			OtherComp->SweepComponent(HitResult, Start, Start + Velocity * 100.f,
			                          OverlappedComponent->GetComponentRotation().Quaternion(),
			                          OverlappedComponent->GetCollisionShape(), true))
		{
			AttachedBone = HitResult.BoneName;
			SetActorLocationAndRotation(HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
		}
		else
		{
			AttachedBone = NAME_None;
			SetActorLocationAndRotation(Start, (-Velocity).Rotation());
		}
	}
	AttachToActor(TargetActor, FAttachmentTransformRules::KeepWorldTransform, AttachedBone);
	RelativeLocation = GetRootComponent()->GetRelativeLocation();
	RelativeRotator = GetRootComponent()->GetRelativeRotation();
}

void AAttachableProjectile::OnRep_IsEnabled()
{
	if (bIsEnabled)
	{
		AttachToActor(TargetActor, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachedBone);
		SetActorRelativeLocation(RelativeLocation);
		SetActorRelativeRotation(RelativeRotator);
	}
}
