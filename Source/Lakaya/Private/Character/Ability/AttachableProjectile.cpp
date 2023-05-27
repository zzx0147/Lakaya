// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/AttachableProjectile.h"

#include "Net/UnrealNetwork.h"

const FName AAttachableProjectile::StaticMeshComponentName = FName("StaticMeshComponent");

void AAttachableProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAttachableProjectile, bIsEnabled);
	DOREPLIFETIME(AAttachableProjectile, TargetActor);
	DOREPLIFETIME(AAttachableProjectile, InitializeLocation);
	DOREPLIFETIME(AAttachableProjectile, AttachedBone);
}

AAttachableProjectile::AAttachableProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(StaticMeshComponentName);
	SetRootComponent(StaticMeshComponent);
}

void AAttachableProjectile::InitializeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                              const FHitResult& SweepResult)
{
	bIsEnabled = true;
	TargetActor = OtherActor;
	InitializeLocation = GetActorLocation();
	AttachedBone = bFromSweep ? SweepResult.BoneName : NAME_None;
	AttachToTargetComponent();
}

void AAttachableProjectile::OnRep_IsEnabled()
{
	if (bIsEnabled) AttachToTargetComponent();
}

void AAttachableProjectile::AttachToTargetComponent()
{
	SetActorLocation(InitializeLocation);
	AttachToActor(TargetActor, FAttachmentTransformRules::KeepWorldTransform, AttachedBone);
}
