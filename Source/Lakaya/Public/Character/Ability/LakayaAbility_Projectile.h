// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LakayaAbility.h"
#include "LakayaAbility_Projectile.generated.h"

/**
 * 오브젝트 풀에 여유 오브젝트가 없고 추가적으로 생성할 수도 없는 상황을 대처하는 정책을 나타냅니다.
 */
UENUM()
enum class EPoolNoObjectPolicy
{
	/** 아무런 조치도 하지 않고 nullptr을 반환합니다. */
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

	bool operator==(const FProjectilePoolItem& Other) const { return Projectile == Other.Projectile; }

	void SetupProjectileItem(FFreeProjectilesArrayType& InFreeProjectiles);

	void UnbindProjectileItem();

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

	~FProjectilePool();

private:
	void InternalAddNewObject();
	void ReFeelExtraObjects();
	ALakayaProjectile* GetFreeProjectile();

	UPROPERTY(Transient)
	TArray<FProjectilePoolItem> Items;

	/**
	 * @brief 풀의 최대 크기를 나타냅니다. 최대 크기 제약이 필요없는 경우 0으로 지정하면 됩니다.
	 * 
	 * 예를 들어 지뢰가 최대 4개만 존재할 수 있도록 하고 싶을 때 이것을 4로 설정해서 4개 이상의 투사체가 생성되지 않도록 할 수 있습니다.
	 */
	UPROPERTY(EditAnywhere, NotReplicated)
	uint32 MaxPoolSize;

	/**
	 * @brief 풀이 몇개의 여분 오브젝트를 보유하도록 할지 지정합니다.
	 * 
	 * 예를 들어 이 값이 2이고 현재 2개의 여유 오브젝트가 존재하는 상황에서
	 * 1개의 오브젝트가 사용되어 여유 오브젝트가 1개로 줄어든다면
	 * 1개의 오브젝트를 더 생성하여 여분 오브젝트가 항상 2개일 수 있도록 합니다.
	 */
	UPROPERTY(EditAnywhere, NotReplicated)
	uint32 MaxExtraObjectCount;

	UPROPERTY(EditAnywhere, NotReplicated)
	EPoolNoObjectPolicy NoExtraPolicy;

	/** 투사체를 스폰하는 함수를 참조하는 델리게이트입니다. */
	FProjectileSpawnDelegate SpawnDelegate;

	/** Collapsed로 전환되어 언제든 사용할 수 있는 투사체들이 여기에 보관됩니다. */
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
