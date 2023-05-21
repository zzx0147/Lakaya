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
	virtual void OnSummoned() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	UFUNCTION()
	virtual void OnRep_SummonedTime();

	UFUNCTION()
	virtual void OnRep_SummonedLocation();

	UFUNCTION()
	virtual void OnRep_SummonedRotation();

	UFUNCTION()
	virtual void OnCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                              const FHitResult& SweepResult);

	float GetServerTime() const;

	UPROPERTY(EditAnywhere)
	float LinearVelocity;

	UPROPERTY(EditAnywhere)
	float BaseDamage;

	UPROPERTY(EditAnywhere)
	float DamageRange;

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(ReplicatedUsing=OnRep_SummonedTime, Transient)
	float SummonedTime;

	UPROPERTY(ReplicatedUsing=OnRep_SummonedLocation, Transient)
	FVector SummonedLocation;

	UPROPERTY(ReplicatedUsing=OnRep_SummonedRotation, Transient)
	FRotator SummonedRotation;
};
