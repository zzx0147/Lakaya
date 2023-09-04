// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

DECLARE_EVENT_ThreeParams(ALakayaProjectile, FProjectileStateChanged,
                          class ALakayaProjectile*, const EProjectileState&, const uint8&)

UCLASS()
class LAKAYA_API ALakayaProjectile : public AActor
{
	GENERATED_BODY()

public:
	ALakayaProjectile();

	void ThrowProjectile();

	FORCEINLINE bool IsCollapsed() const { return ProjectileState == EProjectileState::Collapsed; }
	FORCEINLINE bool IsPerforming() const { return ProjectileState == EProjectileState::Perform; }
	FORCEINLINE bool IsCustomState() const { return ProjectileState == EProjectileState::Custom; }
	FORCEINLINE const EProjectileState& GetProjectileState() const { return ProjectileState; }
	FORCEINLINE const uint8& GetCustomState() const { return CachedCustomState; }

	//TODO: 발사된 시각을 반환하도록 함
	float GetRecentProjectilePerformedTime() const { return 0.0f; }

	/** 투사체의 상태가 변경되면 호출되는 이벤트입니다. 로컬에서 예측적으로 투사체의 상태를 변경할 때에도 호출됩니다. */
	FProjectileStateChanged OnProjectileStateChanged;

	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

protected:
	UFUNCTION(BlueprintNativeEvent)
	void CustomStateChanged(const uint8& OldState);

private:
	UFUNCTION()
	void OnRep_ProjectileState();

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(ReplicatedUsing=OnRep_ProjectileState, Transient)
	EProjectileState ProjectileState;

	/** 투사체에서 추가적인 더 다양한 상태가 필요한 경우 사용됩니다. ProjectileState가 Custom일 때만 리플리케이트됩니다. */
	UPROPERTY(ReplicatedUsing=OnRep_ProjectileState, Transient)
	uint8 CustomState;

	uint8 CachedCustomState;
};
