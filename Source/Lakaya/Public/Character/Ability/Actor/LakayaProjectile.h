// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
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

	FORCEINLINE bool operator==(const FProjectileState& Other) const
	{
		return ProjectileState == Other.ProjectileState && CustomState == Other.CustomState;
	}

	FORCEINLINE bool operator!=(const FProjectileState& Other) const { return !operator==(Other); }

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

	void SetupPredictedProjectileParams(FPredictProjectilePathParams& OutParams, const float& Velocity,
	                                    const float& CurrentTime) const;

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

	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }

	virtual FString ToString() const override { return TEXT("FGameplayAbilityTargetData_ThrowProjectile"); }

	FGameplayAbilityTargetData_ThrowProjectile() = default;

	FGameplayAbilityTargetData_ThrowProjectile(
		FProjectileThrowData&& InThrowData, ALakayaProjectile* const& InProjectile)
		: ThrowData(MoveTemp(InThrowData)), Projectile(InProjectile) { return; }

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

	/**
	 * @brief 투사체를 투척합니다.
	 * @param InThrowData 투사체를 투척하기 위한 데이터입니다.
	 * @param Key 클라이언트에서 투사체가 사용할 예측 키입니다. 이 예측 키가 Reject되면 투사체가 롤백됩니다.
	 */
	void ThrowProjectile(const FProjectileThrowData& InThrowData, FPredictionKey Key);

	/**
	 * @brief 투사체를 Authority를 체크하고 투척합니다. 서버에서만 동작합니다.
	 * @param InThrowData 투사체를 투척하기 위한 데이터입니다.
	 */
	void ThrowProjectileAuthoritative(const FProjectileThrowData& InThrowData);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const FProjectileState& GetProjectileState() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsCollapsed() const { return GetProjectileState().IsCollapsed(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsPerforming() const { return GetProjectileState().IsPerforming(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsCustomState() const { return GetProjectileState().IsCustomState(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetRecentPerformedTime() const { return FMath::Max(ThrowData.ServerTime, RecentPerformedTime); }

	/**
	 * 투사체의 상태가 변경되면 호출되는 이벤트입니다. 로컬에서 예측적으로 투사체의 상태를 변경할 때에도 호출됩니다.
	 * 이 이벤트에서 투사체의 상태를 변경할만한 함수를 호출하면 예기치 못한 버그가 발생할 수 있습니다. (예: ThrowProjectile)
	 */
	FProjectileStateChanged OnProjectileStateChanged;

	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	virtual void PostInitializeComponents() override;

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsActualCollapsed() const { return ProjectileState.IsCollapsed(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsActualPerforming() const { return ProjectileState.IsPerforming(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsActualCustomState() const { return ProjectileState.IsCustomState(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetServerWorldTimeSeconds() const;

	/** 커스텀 스테이트를 해당 값으로 변경합니다. */
	UFUNCTION(BlueprintCallable)
	void SetCustomState(const uint8& InCustomState);

	/** 투사체의 상태를 Collapsed로 변경합니다. */
	UFUNCTION(BlueprintCallable)
	void SetProjectileStateCollapsed();

	//TODO: 추후 블루프린트 라이브러리를 만들어서 옮겨야 합니다.
	UFUNCTION(BlueprintCallable)
	static void SetSourceObject(FGameplayEffectContextHandle EffectContext, const UObject* SourceObject);

	UFUNCTION(BlueprintNativeEvent)
	void OnStartPathPrediction(const FProjectileThrowData& InThrowData);

	UFUNCTION(BlueprintNativeEvent)
	void OnStartPhysicsSimulation(const FPredictProjectilePathResult& LastPredictionResult);

	/** OnRep_ProjectileState에서 커스텀 스테이트에서 탈출할 때 호출됩니다. 0에 대해서는 호출되지 않습니다. */
	UFUNCTION(BlueprintNativeEvent)
	void OnReplicatedCustomStateExit(const uint8& OldState);

	/** OnRep_ProjectileState에서 커스텀 스테이트로 진입할 때 호출됩니다. 0에 대해서는 호출되지 않습니다. */
	UFUNCTION(BlueprintNativeEvent)
	void OnReplicatedCustomStateEnter(const uint8& NewState);

	/** CollisionComponent의 오버랩 이벤트에 바인딩된 함수입니다. 클라이언트에서는 충돌 검사를 신뢰할 수 없으므로 호출되지 않습니다. */
	UFUNCTION(BlueprintNativeEvent)
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                         const FHitResult& SweepResult);

	/**
	 * ProjectileMovementComponent의 바운스 이벤트에 바인딩된 함수입니다.
	 * Block 이벤트는 정적 오브젝트에 대해서만 발동하므로 안전합니다. 따라서 클라이언트에서도 호출됩니다.
	 */
	UFUNCTION(BlueprintNativeEvent)
	void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	/**
	 * 투사체 경로 예측중에 오버랩 이벤트가 발생하면 호출됩니다. 서버에서만 실행됩니다.
	 * @return false를 반환하면 투사체의 진행을 멈춥니다.
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool OnProjectilePathOverlap(const FPredictProjectilePathResult& PredictResult);

	/**
	 * 투사체 경로 예측중에 Block 이벤트가 발생하면 호출됩니다. 서버와 클라이언트 모두에서 실행됩니다.
	 * @return false를 반환하면 투사체의 진행을 멈춥니다.
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool OnProjectilePathBlock(const FPredictProjectilePathResult& PredictResult);

	/**
	 * 투사체 경로 예측중에 FromThrow 이벤트가 트리거되면 호출됩니다. 서버, 클라이언트 모두에서 호출됩니다.
	 * 이 이벤트에서 EventTriggerDelayFromThrow값을 업데이트해서 또 다시 일정시간 뒤에 이벤트가 트리거되도록 할 수 있습니다.
	 * @param Location 이벤트가 트리거되는 시점에서 투사체의 위치입니다.
	 * @param Velocity 이벤트가 트리거되는 시점에서 투사체의 속도입니다.
	 * @return false가 반환되면 투사체의 진행을 멈춥니다.
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool OnEventFromThrowTriggeredInPathPredict(const FVector& Location, const FVector& Velocity);

	/**
	 * 투사체가 물리 시뮬레이션을 통해 투척이 실행되던 도중에 FromThrow 이벤트가 트리거되면 호출됩니다. 서버, 클라이언트 모두에서 호출됩니다.
	 */
	UFUNCTION(BlueprintNativeEvent)
	void OnEventFromThrowTriggeredInPhysics();

	/** 이번 투사체 투척에서 해당 액터에 대한 Overlap 이벤트가 더이상 생성되지 않도록 합니다. */
	UFUNCTION(BlueprintCallable)
	void AddIgnoredInPerformActor(AActor* InActor);

	/** 이번 투사체 투척에서 Ignore되었던 액터들을 다시 Ignore되지 않도록 하고 목록을 비웁니다. */
	UFUNCTION(BlueprintCallable)
	void ClearIgnoredInPerformActors();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EventTriggerDelayFromThrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPredictProjectilePathParams PredictedProjectileParams;

private:
	struct FScopedLock
	{
		explicit FScopedLock(bool& InLockObject) : bLockRef(InLockObject)
		{
			check(!bLockRef);
			bLockRef = true;
		}

		~FScopedLock()
		{
			check(bLockRef);
			bLockRef = false;
		}

	private:
		bool& bLockRef;
	};

	FORCEINLINE FProjectileState& InternalGetProjectileState() { return HasAuthority() ? ProjectileState : LocalState; }
	void ThrowProjectile(const FProjectileThrowData& InThrowData);
	void SetProjectileState(const EProjectileState& InProjectileState);
	void BroadcastOnProjectileStateChanged(const FProjectileState& OldState, const FProjectileState& NewState);
	void RejectProjectile();
	void StopThrowProjectile();
	bool MarchProjectileRecursive(FPredictProjectilePathResult& OutResult, float CurrentTime = 0.f);

	template <class ArgType, class FunType = typename TMemFunPtrType<false, FProjectileState, bool(ArgType)>::Type>
	void InternalSetProjectileState(FunType&& FunPtr, ArgType&& Arg);

	UFUNCTION()
	void OnRep_ProjectileState();

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;

	UPROPERTY(ReplicatedUsing=OnRep_ProjectileState, Transient)
	FProjectileState ProjectileState;

	UPROPERTY(Replicated, Transient)
	FProjectileThrowData ThrowData;

	UPROPERTY(EditAnywhere)
	float ProjectileLaunchVelocity;

	FProjectileState LocalState;
	float RecentPerformedTime;
	bool bIsStateChanging;
	TArray<TWeakObjectPtr<AActor>> IgnoredInPerformActors;
	FTimerHandle EventFromThrowTimerHandle;
};

template <class ArgType, class FunType>
void ALakayaProjectile::InternalSetProjectileState(FunType&& FunPtr, ArgType&& Arg)
{
	auto& StateRef = InternalGetProjectileState();
	const auto OldState = StateRef;
	if ((StateRef.*FunPtr)(Arg))
	{
		BroadcastOnProjectileStateChanged(OldState, StateRef);
	}

	if (OldState.IsPerforming())
	{
		StopThrowProjectile();
	}
}
