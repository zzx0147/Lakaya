// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SummonAbilityInstance.h"
#include "Kismet/GameplayStatics.h"
#include "LinearProjectile.generated.h"

USTRUCT()
struct FTeamCollisionInfo
{
	GENERATED_BODY()

	FTeamCollisionInfo(): ATeamCollision(false), BTeamCollision(false) { return; }

	// 체크하면 A팀 캐릭터에게 충돌합니다.
	UPROPERTY(EditAnywhere)
	bool ATeamCollision;

	// 체크하면 B팀 캐릭터에게 충돌합니다.
	UPROPERTY(EditAnywhere)
	bool BTeamCollision;
};

UCLASS()
class LAKAYA_API ALinearProjectile : public ASummonAbilityInstance
{
	GENERATED_BODY()

public:
	static const FName CollisionComponentName;
	static const FName StaticMeshComponentName;
	static const FName TrailNiagaraComponentName;
	static const FName ExplodeNiagaraComponentName;

	explicit ALinearProjectile(const FObjectInitializer& ObjectInitializer);
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetTeam(const EPlayerTeam& Team) override;

protected:
	virtual void BeginPlay() override;
	virtual void PerformTimerHandler() override;
	virtual void HandleAbilityInstanceReady() override;
	virtual void HandleAbilityInstancePerform() override;
	virtual void HandleAbilityInstanceEnding() override;
	virtual void HandleAbilityInstanceReadyForAction() override;
	virtual void HandleAbilityInstanceAction() override;
	virtual void HandleAbilityInstanceCollapsed() override;

	UFUNCTION()
	virtual void OnCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                              const FHitResult& SweepResult);

private:
	// 물리엔진을 사용하여 투사체를 시뮬레이트합니다.
	void SimulateProjectilePhysics(const bool& UsingQuery = false);

	// 물리엔진을 통한 투사체 시뮬레이션을 종료합니다.
	void DisableProjectilePhysics();

	// 투사체의 초기 위치, 방향, 속도를 바탕으로 현재 시간에 맞는 위치를 시뮬레이트합니다.
	void SimulateProjectileMovement();
	void DisableProjectileSimulation();

	void ShowProjectile();

	void UpdateProjectileTransform();

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
	TEnumAsByte<ECollisionChannel> ATeamCollisionChannel;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> BTeamCollisionChannel;

	// 이 투사체가 어느 팀일 때 어느 팀 캐릭터에 충돌할지 결정합니다. 지정되지 않은 경우 충돌하지 않습니다.
	UPROPERTY(EditAnywhere)
	TMap<EPlayerTeam, FTeamCollisionInfo> TeamCollisionMap;

	UPROPERTY(EditAnywhere)
	FPredictProjectilePathParams ProjectilePathParams;

	// 최초로 충돌한 직후 자동으로 Ending으로 넘어가도록 할지 지정합니다.
	UPROPERTY(EditAnywhere)
	bool bAutoEnding;

	// AutoEnding으로 인해 Ending으로 진입한 이후, Ending에서 스태틱 메시를 숨길지 여부를 결정합니다.
	UPROPERTY(EditAnywhere)
	bool bHideMeshOnEnding;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* CollisionNiagara;

private:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* TrailNiagaraComponent;

	UPROPERTY(Replicated, Transient)
	FVector ProjectileLocation;

	UPROPERTY(Replicated, Transient)
	FRotator ProjectileRotation;

	FPredictProjectilePathResult ProjectilePathResult;
	float RecentPathCalculateTime;
	FPredictProjectilePathPointData RecentPointData;
	EPlayerTeam RecentTeam;
};
