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

void FProjectilePoolItem::SetupProjectileItem(FFreeProjectilesArrayType& InFreeProjectiles)
{
	if (!ensure(Projectile))
	{
		return;
	}

	OnProjectileStateChangedHandle = Projectile->OnProjectileStateChanged.AddLambda(
		[&InFreeProjectiles](ALakayaProjectile* InProjectile, const EProjectileState& NewState, const uint8&)
		{
			NewState == EProjectileState::Collapsed
				? InFreeProjectiles.AddUnique(InProjectile)
				: InFreeProjectiles.Remove(InProjectile);
		});

	if (Projectile->IsCollapsed())
	{
		InFreeProjectiles.AddUnique(Projectile);
	}
}

void FProjectilePoolItem::PostReplicatedAdd(const FProjectilePool& InArray)
{
	// We does not modify 'Items' so It's safe to cast away const.
	auto& TargetArray = const_cast<FFreeProjectilesArrayType&>(InArray.FreeProjectiles);
	SetupProjectileItem(TargetArray);
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

	auto& Item = Items[Items.Emplace(Instance)];
	MarkItemDirty(Item);
	Item.SetupProjectileItem(FreeProjectiles);
}

ULakayaAbility_Projectile::ULakayaAbility_Projectile(): ProjectilePool()
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void ULakayaAbility_Projectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULakayaAbility_Projectile, ProjectilePool);
}
