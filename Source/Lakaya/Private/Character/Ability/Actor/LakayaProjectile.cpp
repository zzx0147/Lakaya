// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Actor/LakayaProjectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"

ALakayaProjectile::ALakayaProjectile()
{
	SetReplicates(true);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("ProjectileMovementComponent"));
}

void ALakayaProjectile::ThrowProjectile()
{
}

void ALakayaProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALakayaProjectile, ProjectileState);
	DOREPLIFETIME(ALakayaProjectile, CustomState);
}

void ALakayaProjectile::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
	DOREPLIFETIME_ACTIVE_OVERRIDE(ALakayaProjectile, CustomState, IsCustomState());
}

void ALakayaProjectile::OnRep_ProjectileState()
{
}
