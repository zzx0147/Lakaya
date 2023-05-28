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

void AAttachableProjectile::InitializeHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	bIsEnabled = true;
	TargetActor = OtherActor;
	AttachedBone = Hit.BoneName;
	SetActorLocationAndRotation(Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
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
