// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SummonAbilityInstance.h"
#include "LinearProjectile.generated.h"

UCLASS()
class LAKAYA_API ALinearProjectile : public ASummonAbilityInstance
{
	GENERATED_BODY()

public:
	static const FName SceneComponentName;
	static const FName CollisionComponentName;
	static const FName StaticMeshComponentName;

	explicit ALinearProjectile(const FObjectInitializer& ObjectInitializer);
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetTeam(const EPlayerTeam& Team) override;

protected:
	virtual void PerformTimerHandler() override;
	virtual void HandleAbilityInstancePerform() override;
	virtual void HandleAbilityInstanceEnding() override;

	UFUNCTION()
	virtual void OnCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                              const FHitResult& SweepResult);

private:
	/**
	 * @brief 물리엔진을 사용하여 투사체를 시뮬레이트합니다.
	 * @param CollisionQueryEnabled 투사체가 충돌 이벤트를 생성할지 여부입니다.
	 * @param UpdateProjectileTransform 투사체가 ProjectileLocation, ProjectileRotation을 업데이트할지 여부입니다.
	 */
	void SimulateProjectilePhysics(const bool& CollisionQueryEnabled, const bool& UpdateProjectileTransform);

	// 물리엔진을 통한 투사체 시뮬레이션을 종료합니다.
	void DisableProjectilePhysics();

	// 투사체의 초기 위치, 방향, 속도를 바탕으로 현재 시간에 맞는 위치를 시뮬레이트합니다.
	void SimulateProjectileMovement();

	// 투사체 위치 시뮬레이션을 종료합니다.
	void DisableProjectileSimulation();

protected:
	UPROPERTY(EditAnywhere)
	float LinearVelocity;

	UPROPERTY(EditAnywhere)
	float BaseDamage;

	// 데미지 범위를 지정합니다. 0이거나 음수인 경우 단일피해로 적용됩니다.
	UPROPERTY(EditAnywhere)
	float DamageRange;

	// 투사체의 트레일 나이아가라 시스템을 지정합니다.
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailNiagaraSystem;

	// 투사체가 어떤 물체에 충돌한 후 트리거 되는 나이아가라 시스템을 지정합니다.
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* CollisionNiagaraSystem;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> ATeamCollisionChannel;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> BTeamCollisionChannel;

private:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(Replicated, Transient)
	FVector ProjectileLocation;

	UPROPERTY(Replicated, Transient)
	FRotator ProjectileRotation;

	TWeakObjectPtr<class UNiagaraComponent> TrailNiagara;
};
