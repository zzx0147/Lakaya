// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameFramework/Actor.h"
#include "LakayaProjectile.generated.h"

UENUM(BlueprintType)
enum class EProjectileState : uint8
{
	/** 투사체가 게임에 아무런 개입도 하지 않고 숨겨져 있는 상태입니다. */
	Collapsed,

	/** 투사체가 발사되어 날아가는 중입니다. */
	Perform,

	/** 하위 클래스에서 이 외의 다른 상태들이 필요한 경우 사용합니다. */
	Custom
};

USTRUCT(BlueprintType)
struct FProjectileState
{
	GENERATED_BODY()

	FProjectileState() = default;

	//TODO: 필요시 BlueprintLibrary 만들어야함
	FORCEINLINE const EProjectileState& GetProjectileState() const { return ProjectileState; }
	FORCEINLINE bool IsCollapsed() const { return GetProjectileState() == EProjectileState::Collapsed; }
	FORCEINLINE bool IsPerforming() const { return GetProjectileState() == EProjectileState::Perform; }
	FORCEINLINE bool IsCustomState() const { return GetProjectileState() == EProjectileState::Custom; }
	FORCEINLINE const uint8& GetCustomState() const { return CustomState; }

	/**
	 * @brief 투사체의 상태를 변경합니다.
	 * @return 변경된 내용이 있는 경우 true를 반환합니다.
	 */
	bool SetProjectileState(const EProjectileState& InProjectileState);

	/**
	 * @brief 투사체의 커스텀 상태를 변경합니다.
	 * @return 변경된 내용이 있는 경우 true를 반환합니다.
	 */
	bool SetCustomState(const uint8& InCustomState);

	bool operator==(const FProjectileState& Other) const
	{
		return ProjectileState == Other.ProjectileState && CustomState == Other.CustomState;
	}

	bool operator!=(const FProjectileState& Other) const { return !operator==(Other); }

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

private:
	UPROPERTY()
	EProjectileState ProjectileState;

	UPROPERTY()
	uint8 CustomState;
};

template <>
struct TStructOpsTypeTraits<FProjectileState> : public TStructOpsTypeTraitsBase2<FProjectileState>
{
	enum
	{
		WithNetSerializer = true,
	};
};

DECLARE_EVENT_ThreeParams(ALakayaProjectile, FProjectileStateChanged, class ALakayaProjectile*,
                          const FProjectileState& /* OldState */, const FProjectileState& /* NewState */);

USTRUCT(BlueprintType)
struct FProjectileThrowData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FVector_NetQuantize100 ThrowLocation;

	UPROPERTY(BlueprintReadWrite)
	FVector_NetQuantizeNormal ThrowDirection;

	UPROPERTY(BlueprintReadWrite)
	float ServerTime;

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, FProjectileThrowData& ThrowData)
	{
		Ar << ThrowData.ThrowLocation << ThrowData.ThrowDirection << ThrowData.ServerTime;
		return Ar;
	}
};

USTRUCT(BlueprintType)
struct FGameplayAbilityTargetData_ThrowProjectile : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FProjectileThrowData ThrowData;

	UPROPERTY(BlueprintReadWrite)
	ALakayaProjectile* Projectile;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template <>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_ThrowProjectile>
	: public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_ThrowProjectile>
{
	enum
	{
		// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
		WithNetSerializer = true
	};
};

UCLASS()
class LAKAYA_API ALakayaProjectile : public AActor
{
	GENERATED_BODY()

public:
	ALakayaProjectile();

	void ThrowProjectilePredictive(FPredictionKey& Key, const FProjectileThrowData& InThrowData);
	void ThrowProjectileAuthoritative(FProjectileThrowData&& InThrowData);

	/** 투사체를 비활성화합니다. 서버에서만 사용할 수 있습니다. */
	void CollapseProjectile();

	FORCEINLINE const FProjectileState& GetProjectileState() const
	{
		return HasAuthority() ? ProjectileState : LocalState;
	}

	FORCEINLINE FProjectileState& GetProjectileState() { return HasAuthority() ? ProjectileState : LocalState; }
	FORCEINLINE bool IsCollapsed() const { return GetProjectileState().IsCollapsed(); }
	FORCEINLINE bool IsPerforming() const { return GetProjectileState().IsPerforming(); }
	FORCEINLINE bool IsCustomState() const { return GetProjectileState().IsCustomState(); }

	const float& GetRecentProjectilePerformedTime() const { return RecentProjectilePerformedTime; }

	/** 투사체의 상태가 변경되면 호출되는 이벤트입니다. 로컬에서 예측적으로 투사체의 상태를 변경할 때에도 호출됩니다. */
	FProjectileStateChanged OnProjectileStateChanged;

	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

protected:
	FORCEINLINE bool IsActualCollapsed() const { return ProjectileState.IsCollapsed(); }
	FORCEINLINE bool IsActualPerforming() const { return ProjectileState.IsPerforming(); }
	FORCEINLINE bool IsActualCustomState() const { return ProjectileState.IsCustomState(); }

	void SetCustomState(const uint8& InCustomState);

	virtual void OnRep_CustomState();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnCollapsed();

private:
	struct FScopedLock
	{
		explicit FScopedLock(bool& InLockObject) : bLockRef(InLockObject)
		{
			if (ensureMsgf(!bLockRef, TEXT("Recursive call detected")))
			{
				bLockRef = true;
			}
		}

		~FScopedLock()
		{
			ensure(bLockRef);
			bLockRef = false;
		}

	private:
		bool& bLockRef;
	};
	
	void ThrowProjectile(const FProjectileThrowData& InThrowData);
	void SetProjectileState(const EProjectileState& InProjectileState);
	void BroadcastOnProjectileStateChanged(const FProjectileState& OldState, const FProjectileState& NewState);

	UFUNCTION()
	void OnRep_ProjectileState();

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(ReplicatedUsing=OnRep_ProjectileState, Transient)
	FProjectileState ProjectileState;

	UPROPERTY(Replicated, Transient)
	FProjectileThrowData ThrowData;

	FProjectileState LocalState;
	float RecentProjectilePerformedTime;
	bool bIsStateChanging;

	friend struct FScopedLock;
};
