// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_Projectile.h"

#include "Algo/Accumulate.h"
#include "Character/Ability/Actor/LakayaProjectile.h"
#include "Net/UnrealNetwork.h"

FProjectilePoolItem::FProjectilePoolItem(ALakayaProjectile* InProjectile,
                                         FFreeProjectilesArrayType& InFreeProjectiles) : Projectile(InProjectile)
{
	SetupProjectileItem(InFreeProjectiles);
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

void FProjectilePoolItem::UnbindProjectileItem()
{
	if (OnProjectileStateChangedHandle.IsValid())
	{
		if (Projectile)
		{
			Projectile->OnProjectileStateChanged.Remove(OnProjectileStateChangedHandle);
		}
		OnProjectileStateChangedHandle.Reset();
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
	ReFeelExtraObjects();
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

FProjectilePool::~FProjectilePool()
{
	for (auto&& Item : Items)
	{
		Item.UnbindProjectileItem();
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
	Instance->SetReplicates(true);

	auto& Item = Items[Items.Emplace(Instance, FreeProjectiles)];
	MarkItemDirty(Item);
}

void FProjectilePool::ReFeelExtraObjects()
{
	const auto RemainExtraCount = MaxExtraObjectCount - FreeProjectiles.Num();
	const auto FixedSpawnCount = MaxPoolSize > 0
		                             ? FMath::Min(RemainExtraCount, MaxPoolSize - Items.Num())
		                             : RemainExtraCount;

	for (auto Count = 0; Count < FixedSpawnCount; ++Count)
	{
		InternalAddNewObject();
	}
}

ALakayaProjectile* FProjectilePool::GetFreeProjectile()
{
	static const auto GetOlderProjectile = [](ALakayaProjectile* A, ALakayaProjectile* B)
	{
		return A->GetRecentProjectilePerformedTime() < B->GetRecentProjectilePerformedTime() ? A : B;
	};
	
	static const auto ProjectileReduce = [](ALakayaProjectile* Recent, const FProjectilePoolItem& Item)
	{
		return !Recent ? Item.Projectile : !Item.Projectile ? Recent : GetOlderProjectile(Recent, Item.Projectile);
	};
	
	ALakayaProjectile* Projectile = nullptr;

	if (FreeProjectiles.IsEmpty())
	{
		// 투사체는 실제로 투사체가 사용되기 시작할 때 생성되며 여기에서 생성하지 않습니다.
		if (NoExtraPolicy == EPoolNoObjectPolicy::RecycleOldest)
		{
			// 로컬 클라이언트에서 예측적으로 동작하는 것일 수도 있으므로 여기에서 투사체를 Collapse 시키지는 않습니다.
			Projectile = Algo::Accumulate(Items, Projectile, ProjectileReduce);
		}
	}
	else
	{
		Projectile = FreeProjectiles.Top().Get();
	}

	return Projectile;
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
