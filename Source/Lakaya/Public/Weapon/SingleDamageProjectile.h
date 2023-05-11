// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SingleDamageProjectile.generated.h"

UCLASS()
class LAKAYA_API ASingleDamageProjectile : public AActor
{
	GENERATED_BODY()

public:
	static const FName SphereComponentName;
	static const FName StaticMeshComponentName;
	static const FName MovementComponentName;

	ASingleDamageProjectile(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:
	virtual void SetDamage(const float& ArgDamage);

private:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* MovementComponent;

	float Damage;
};
