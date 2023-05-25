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
	void SimulateProjectilePhysics();

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
