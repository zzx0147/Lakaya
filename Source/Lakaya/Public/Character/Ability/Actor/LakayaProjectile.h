// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LakayaProjectile.generated.h"

UENUM(BlueprintType)
enum class EProjectileState : uint8
{
	// 투사체가 게임에 아무런 개입도 하지 않고 숨겨져 있는 상태입니다.
	Collapsed,

	// 투사체가 발사되어 날아가는 중입니다.
	Perform,

	// 하위 클래스에서 이 외의 다른 상태들이 필요한 경우 사용합니다.
	Custom
};

DECLARE_EVENT_ThreeParams(ALakayaProjectile, FProjectileStateChanged, class ALakayaProjectile*, const EProjectileState&,
                          const uint8&)

UCLASS()
class LAKAYA_API ALakayaProjectile : public AActor
{
	GENERATED_BODY()

public:
	ALakayaProjectile();

	void ThrowProjectile();
	FORCEINLINE bool IsCustomState() const { return ProjectileState == EProjectileState::Custom; }
	FORCEINLINE bool IsCollapsed() const { return ProjectileState == EProjectileState::Collapsed; }
	FORCEINLINE const EProjectileState& GetProjectileState() const { return ProjectileState; }

	FProjectileStateChanged OnProjectileStateChanged;

	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

protected:

private:
	UFUNCTION()
	void OnRep_ProjectileState();

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(ReplicatedUsing=OnRep_ProjectileState, Transient)
	EProjectileState ProjectileState;

	UPROPERTY(Replicated, Transient)
	uint8 CustomState;
};
