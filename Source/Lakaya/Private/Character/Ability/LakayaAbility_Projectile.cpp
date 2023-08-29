// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_Projectile.h"

#include "Character/Ability/Actor/LakayaProjectile.h"
#include "Net/UnrealNetwork.h"

FProjectilePoolItem::~FProjectilePoolItem()
{
	// 이동생성시 문제 가능성
	if (Projectile)
	{
		Projectile->OnProjectileStateChanged.Remove(OnProjectileStateChangedHandle);
	}
}

void FProjectilePoolItem::PreReplicatedRemove(const FProjectilePool& InArray)
{
}

void FProjectilePoolItem::PostReplicatedAdd(const FProjectilePool& InArray)
{
}

void FProjectilePool::Initialize(FProjectileSpawnDelegate InSpawnDelegate)
{
	check(InSpawnDelegate.IsBound());
	SpawnDelegate = InSpawnDelegate;
	for (auto Count = 0; Count < MaxExtraObjectCount; ++Count)
	{
		InternalAddNewObject();
	}
}

bool FProjectilePool::IsMaximumReached() const
{
	return Items.Num() >= MaxPoolSize;
}

bool FProjectilePool::IsExtraObjectMaximumReached() const
{
	return FreeProjectiles.Num() >= MaxExtraObjectCount;
}

void FProjectilePool::AddNewObject()
{
	if (!IsMaximumReached() && !IsExtraObjectMaximumReached())
	{
		InternalAddNewObject();
	}
}

void FProjectilePool::InternalAddNewObject()
{
	if (!ensure(SpawnDelegate.IsBound()))
	{
		return;
	}

	const auto Instance = SpawnDelegate.Execute();
	if (!ensure(Instance))
	{
		return;
	}

	//TODO: MarkDirty
	auto& Item = Items[Items.Emplace(Instance)];

	Item.OnProjectileStateChangedHandle = Instance->OnProjectileStateChanged.AddLambda(
		[this](ALakayaProjectile* Projectile, const EProjectileState& NewState, const uint8& NewCustomState)
		{
			NewState == EProjectileState::Collapsed
				? FreeProjectiles.AddUnique(Projectile)
				: FreeProjectiles.Remove(Projectile);
		});

	if (!Instance->IsCollapsed())
	{
		FreeProjectiles.AddUnique(Instance);
	}
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
