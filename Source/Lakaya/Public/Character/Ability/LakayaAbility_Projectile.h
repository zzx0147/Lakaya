// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LakayaAbility.h"
#include "LakayaAbility_Projectile.generated.h"

USTRUCT()
struct FProjectilePoolItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	class ALakayaProjectile* Projectile;

	void PreReplicatedRemove(const struct FProjectilePool& InArray);
	void PostReplicatedAdd(const FProjectilePool& InArray);
};

USTRUCT()
struct FProjectilePool : public FFastArraySerializer
{
	GENERATED_BODY()

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FProjectilePoolItem, FProjectilePool>(Items, DeltaParms, *this);
	}

private:
	UPROPERTY()
	TArray<FProjectilePoolItem> Items;

	TArray<TWeakObjectPtr<ALakayaProjectile>> FreeProjectiles;

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
