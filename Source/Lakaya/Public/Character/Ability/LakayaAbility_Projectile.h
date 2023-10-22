// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LakayaAbility.h"
#include "Actor/LakayaProjectile.h"
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

using FFreeProjectilesArrayType = TArray<TWeakObjectPtr<ALakayaProjectile>>;

USTRUCT()
struct FProjectilePoolItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FProjectilePoolItem() = default;
	FProjectilePoolItem(ALakayaProjectile* InProjectile) : Projectile(InProjectile) { return; }
	FProjectilePoolItem(ALakayaProjectile* InProjectile, const FProjectileStateChanged::FDelegate& Delegate);

	bool operator==(const FProjectilePoolItem& Other) const { return Projectile == Other.Projectile; }

	void BindProjectileItem(const FProjectileStateChanged::FDelegate& Delegate);

	void UnbindProjectileItem();

	UPROPERTY()
	ALakayaProjectile* Projectile;

	void PostReplicatedAdd(const struct FProjectilePool& InArray);
	void PostReplicatedRemove(const struct FProjectilePool& InArray);

private:
	FDelegateHandle OnProjectileStateChangedHandle;

	friend struct FProjectilePool;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProjectilePoolProjectileEventSignature, ALakayaProjectile*, Projectile);

USTRUCT()
struct FProjectilePool : public FFastArraySerializer
{
	GENERATED_BODY()

	void Initialize(UWorld* InSpawnWorld, const FActorSpawnParameters& InActorSpawnParameters);

	FORCEINLINE bool IsMaximumReached() const
	{
		return MaxPoolSize != 0 && static_cast<uint32>(Items.Num()) >= MaxPoolSize;
	}

	FORCEINLINE bool IsExtraObjectMaximumReached() const
	{
		return static_cast<uint32>(FreeProjectiles.Num()) >= MaxExtraObjectCount;
	}

	/** 사용 가능한 투사체가 존재하는지 여부입니다. */
	bool IsAvailable() const;

	/** 새로운 투사체를 하나 추가합니다. 단 조건이 만족되는 경우에만 추가됩니다. */
	void AddNewObject();

	/**
	 * 사용 가능한 투사체를 가져옵니다. 이 함수를 통해 투사체를 가져오더라도 풀에서 제거되는 것은 아닙니다.
	 * ThrowProjectile 등의 함수를 통해 투사체를 사용하고 나서 다시 GetFreeProjectile을 호출하세요.
	 * 한 스코프에서 이 함수를 반복적으로 호출한다면 모두 같은 투사체를 참조하게될 수 있습니다.
	 */
	ALakayaProjectile* GetFreeProjectile();

	/** 지정한 투사체가 존재하는 경우 제거하고 새로 하나를 스폰합니다. */
	bool RemoveProjectile(ALakayaProjectile* Projectile);

	/** 모든 투사체를 제거합니다. 다시 Initialize를 호출하기 전까지 투사체 풀은 아무 것도 하지 않습니다. */
	void ClearProjectilePool();

	void SetInstigator(APawn* Instigator);

	/** 투사체가 스폰되고 난 직후 호출되는 이벤트입니다. */
	UPROPERTY(BlueprintAssignable, NotReplicated)
	FProjectilePoolProjectileEventSignature OnProjectileSpawned;

	/** 투사체가 풀 내부에서 Destroy시키기 직전에 호출됩니다. */
	UPROPERTY(BlueprintAssignable, NotReplicated)
	FProjectilePoolProjectileEventSignature OnPreProjectileDestroy;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FProjectilePoolItem, FProjectilePool>(Items, DeltaParms, *this);
	}

	~FProjectilePool();

private:
	void InternalAddNewObject();
	void ReFeelExtraObjects();

	FORCEINLINE auto CreateProjectileStateDelegate(
		const FProjectileStateChanged::FDelegate::TMethodPtr<FProjectilePool>& MemberFunction)
	{
		return FProjectileStateChanged::FDelegate::CreateRaw(this, MemberFunction);
	}

	FORCEINLINE auto CreateClientProjectileStateDelegate()
	{
		return CreateProjectileStateDelegate(&FProjectilePool::ClientProjectileStateChanged);
	}

	FORCEINLINE auto CreateServerProjectileStateDelegate()
	{
		return CreateProjectileStateDelegate(&FProjectilePool::ServerProjectileStateChanged);
	}

	void ClientProjectileStateChanged(ALakayaProjectile* InProjectile, const FProjectileState& OldState,
	                                  const FProjectileState& NewState);
	void ServerProjectileStateChanged(ALakayaProjectile* InProjectile, const FProjectileState& OldState,
	                                  const FProjectileState& NewState);

	UPROPERTY(Transient)
	TArray<FProjectilePoolItem> Items;

	UPROPERTY(EditAnywhere, NotReplicated)
	TSubclassOf<ALakayaProjectile> ProjectileClass;

	/**
	 * @brief 풀의 최대 크기를 나타냅니다. 최대 크기 제약이 필요없는 경우 0으로 지정하면 됩니다.
	 * 
	 * 예를 들어 지뢰가 최대 4개만 존재할 수 있도록 하고 싶을 때 이것을 4로 설정해서 4개 이상의 투사체가 생성되지 않도록 할 수 있습니다.
	 */
	UPROPERTY(EditAnywhere, NotReplicated)
	uint32 MaxPoolSize;

	/**
	 * @brief 풀이 몇개의 여분 오브젝트를 보유하도록 할지 지정합니다.
	 * 클라이언트에서 아주 빠른 간격으로 여러개의 투사체가 사용되는 경우 이 값을 높게 설정하는 것이 좋습니다.
	 * 
	 * 예를 들어 이 값이 2이고 현재 2개의 여유 오브젝트가 존재하는 상황에서
	 * 1개의 오브젝트가 사용되어 여유 오브젝트가 1개로 줄어든다면
	 * 1개의 오브젝트를 더 생성하여 여분 오브젝트가 항상 2개일 수 있도록 합니다.
	 */
	UPROPERTY(EditAnywhere, NotReplicated)
	uint32 MaxExtraObjectCount;

	UPROPERTY(EditAnywhere, NotReplicated)
	EPoolNoObjectPolicy NoExtraPolicy;

	TWeakObjectPtr<UWorld> SpawnWorld;
	FActorSpawnParameters ActorSpawnParameters;

	/** Collapsed로 전환되어 언제든 사용할 수 있는 투사체들이 여기에 보관됩니다. */
	mutable FFreeProjectilesArrayType FreeProjectiles;

	friend struct FProjectilePoolItem;
	friend class ULakayaAbility_Projectile;
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
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	                                FGameplayTagContainer* OptionalRelevantTags) const override;

protected:
	virtual void OnTargetDataReceived_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	                                                 FGameplayTag GameplayTag) override;
	virtual FGameplayAbilityTargetDataHandle MakeTargetData_Implementation() override;

	UFUNCTION(BlueprintNativeEvent)
	void MakeProjectileThrowLocation(FVector& OutLocation, FVector& OutDirection);

	void MakeProjectileThrowLocation_Implementation(FVector& OutLocation, FVector& OutDirection)
	PURE_VIRTUAL(&ThisClass::MakeProjectileThrowLocation_Implementation,)

	UFUNCTION()
	virtual void OnProjectileSpawned(ALakayaProjectile* Projectile);

	UFUNCTION()
	virtual void OnPreProjectileDestroy(ALakayaProjectile* Projectile);

	UFUNCTION()
	virtual void OnProjectileDestroyed(AActor* Projectile);

private:
	UPROPERTY(Replicated, EditAnywhere)
	FProjectilePool ProjectilePool;
};
