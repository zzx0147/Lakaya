// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_Projectile.h"

#include "Net/UnrealNetwork.h"

void FProjectilePoolItem::PreReplicatedRemove(const FProjectilePool& InArray)
{
}

void FProjectilePoolItem::PostReplicatedAdd(const FProjectilePool& InArray)
{
}

ULakayaAbility_Projectile::ULakayaAbility_Projectile()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void ULakayaAbility_Projectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULakayaAbility_Projectile, ProjectilePool);
}
