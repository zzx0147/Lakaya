// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SummonAbilityInstance.h"
#include "Kismet/GameplayStatics.h"
#include "LinearProjectile.generated.h"

UCLASS()
class LAKAYA_API ALinearProjectile : public ASummonAbilityInstance
{
	GENERATED_BODY()

public:
	static const FName CollisionComponentName;
	static const FName MeshComponentName;
	static const FName TrailNiagaraComponentName;

	explicit ALinearProjectile(const FObjectInitializer& ObjectInitializer);
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetTeam(const ETeam& Team) override;

protected:
	virtual void BeginPlay() override;
	virtual void PerformTimerHandler() override;
	virtual void HandleAbilityInstanceReady() override;
	virtual void HandleAbilityInstancePerform() override;
	virtual void HandleAbilityInstanceEnding() override;
	virtual void HandleAbilityInstanceCollapsed() override;
	virtual void HandleReadyStateExit() override;
	virtual void HandlePerformStateExit() override;
	virtual void HandleEndingStateExit() override;
	virtual void HandleCollapsedStateExit() override;

	UFUNCTION()
	virtual void OnCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                              const FHitResult& SweepResult);

	void UpdateProjectileTransform();

	FORCEINLINE UMeshComponent* GetMeshComponent() const { return MeshComponent; }

	// 물리엔진을 통한 투사체 시뮬레이션을 종료합니다.
	void DisableProjectilePhysics();

	/**
	 * @brief Team에 따라 컴포넌트의 충돌 반응을 결정합니다.
	 * @param PrimitiveComponent 대상 컴포넌트입니다.
	 * @param Team 기준 팀입니다.
	 * @param WithAlly 아군과 충돌할지 여부를 결정합니다.
	 * @param WithEnemy 적과 충돌할지 여부를 결정합니다.
	 */
	void SetTeamCollisionResponse(UPrimitiveComponent* PrimitiveComponent, const ETeam& Team,
	                              const bool& WithAlly, const bool& WithEnemy) const;

	void SetIgnoreInstigator(UPrimitiveComponent* PrimitiveComponent);

private:
	// 물리엔진을 사용하여 투사체를 시뮬레이트합니다.
	void SimulateProjectilePhysics(const bool& UsingQuery = false, const bool& UpdateProjectile = false);

	// 투사체의 초기 위치, 방향, 속도를 바탕으로 현재 시간에 맞는 위치를 시뮬레이트합니다.
	void SimulateProjectileMovement();

	void CalculateProjectilePath(const FVector& Location, const FRotator& Rotator);
	void RecalculateProjectilePath();

	static bool CustomPointDataPredicate(const FPredictProjectilePathPointData& First,
	                                     const FPredictProjectilePathPointData& Second);

	UFUNCTION(NetMulticast, Reliable)
	void NotifyCollision(const FVector& Location, const FVector& Direction);

protected:
	UPROPERTY(EditAnywhere)
	float LinearVelocity;

	// 투사체가 폭발할 때의 최대 피해량을 지정합니다.
	UPROPERTY(EditAnywhere)
	float BaseDamage;

	// 폭발의 범위를 지정합니다. 0인 경우 최초 접촉한 물체에 대한 단일피해로 적용되며, 음수인 경우 피해를 적용하지 않습니다.
	UPROPERTY(EditAnywhere)
	float DamageRange;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> IndividualCollisionChannel;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> ATeamCollisionChannel;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> BTeamCollisionChannel;

	// 이 투사체를 소환한 캐릭터와 충돌하도록 할지 결정합니다.
	UPROPERTY(EditAnywhere)
	bool bInstigatorCollision;

	// 아군 캐릭터와 충돌하도록 할지 결정합니다. 개인전에서는 사용되지 않습니다.
	UPROPERTY(EditAnywhere)
	bool bAllyCollision;

	// 적 캐릭터와 충돌하도록 할지 결정합니다.
	UPROPERTY(EditAnywhere)
	bool bEnemyCollision;

	UPROPERTY(EditAnywhere)
	FPredictProjectilePathParams ProjectilePathParams;

	// 최초로 충돌한 직후 자동으로 충돌 위치에 정지하고 Ending으로 넘어가도록 할지 지정합니다.
	UPROPERTY(EditAnywhere)
	bool bAutoEnding;

	// AutoEnding으로 인해 Ending으로 진입한 이후, Ending에서 스태틱 메시를 숨길지 여부를 결정합니다.
	UPROPERTY(EditAnywhere)
	bool bHideMeshOnEnding;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* CollisionNiagara;

	UPROPERTY(Replicated, Transient)
	FVector ProjectileLocation;

	UPROPERTY(Replicated, Transient)
	FRotator ProjectileRotation;

private:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere)
	UMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* TrailNiagaraComponent;

	FPredictProjectilePathResult ProjectilePathResult;
	float RecentPathCalculateTime;
	FPredictProjectilePathPointData RecentPointData;

	UPROPERTY(EditAnywhere, Category = Debug)
	uint8 EnableDebugDraw : 1;
};
