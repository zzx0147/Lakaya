// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LakayaAbility.h"
#include "LakayaAbility_Projectile.generated.h"

/**
 * 오브젝트 풀에 여유 오브젝트가 없을 때의 정책을 정의합니다.
 */
UENUM()
enum class EPoolNoObjectPolicy
{
	/** 아무런 조치를 하지 않고, nullptr을 반환하게끔 합니다. */
	ReturnNull,

	/** 가장 오래된 오브젝트를 재활용합니다. */
	RecycleOldest
};

DECLARE_DELEGATE_RetVal(class ALakayaProjectile*, FProjectileSpawnDelegate)

using FFreeProjectilesArrayType = TArray<TWeakObjectPtr<ALakayaProjectile>>;

USTRUCT()
struct FProjectilePoolItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FProjectilePoolItem() = default;
	FProjectilePoolItem(ALakayaProjectile* InProjectile) : Projectile(InProjectile) { return; }
	FProjectilePoolItem(ALakayaProjectile* InProjectile, FFreeProjectilesArrayType& InFreeProjectiles);
	~FProjectilePoolItem();

	bool operator==(const FProjectilePoolItem& Other) const { return Projectile == Other.Projectile; }

	void SetupProjectileItem(FFreeProjectilesArrayType& InFreeProjectiles);

	UPROPERTY()
	ALakayaProjectile* Projectile;

	void PostReplicatedAdd(const struct FProjectilePool& InArray);

private:
	FDelegateHandle OnProjectileStateChangedHandle;

	friend struct FProjectilePool;
};

USTRUCT()
struct FProjectilePool : public FFastArraySerializer
{
	GENERATED_BODY()

	void Initialize(FProjectileSpawnDelegate InSpawnDelegate);
	bool IsMaximumReached() const;
	bool IsExtraObjectMaximumReached() const;
	void AddNewObject();

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FProjectilePoolItem, FProjectilePool>(Items, DeltaParms, *this);
	}

private:
	void InternalAddNewObject();
	void ReFeelExtraObjects();

	UPROPERTY(Transient)
	TArray<FProjectilePoolItem> Items;

	UPROPERTY(EditAnywhere, NotReplicated)
	int32 MaxPoolSize;

	UPROPERTY(EditAnywhere, NotReplicated)
	uint16 MaxExtraObjectCount;

	UPROPERTY(EditAnywhere, NotReplicated)
	EPoolNoObjectPolicy NoExtraPolicy;

	FProjectileSpawnDelegate SpawnDelegate;
	FFreeProjectilesArrayType FreeProjectiles;

	friend struct FProjectilePoolItem;
};

template <>
struct TStructOpsTypeTraits<FProjectilePool> : public TStructOpsTypeTraitsBase2<FProjectilePool>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

/**
 * 
 */
UCLASS()
class LAKAYA_API ULakayaAbility_Projectile : public ULakayaAbility
{
	GENERATED_BODY()

public:
	ULakayaAbility_Projectile();

private:
	UPROPERTY(Replicated, EditAnywhere)
	FProjectilePool ProjectilePool;
};
