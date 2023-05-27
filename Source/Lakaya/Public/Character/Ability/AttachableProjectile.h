// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttachableProjectile.generated.h"

UCLASS()
class LAKAYA_API AAttachableProjectile : public AActor
{
	GENERATED_BODY()

public:
	AAttachableProjectile();
	virtual void InitializeHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                           FVector NormalImpulse, const FHitResult& Hit);

protected:
	UFUNCTION()
	virtual void OnRep_IsEnabled();

private:
	UPROPERTY(ReplicatedUsing=OnRep_IsEnabled, Transient)
	bool bIsEnabled;

	UPROPERTY(Replicated, Transient)
	AActor* TargetActor;

	UPROPERTY(Replicated, Transient)
	FVector RelativeLocation;

	UPROPERTY(Replicated, Transient)
	FRotator RelativeRotator;

	UPROPERTY(Replicated, Transient)
	FName AttachedBone;
};
