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
	static const FName StaticMeshComponentName;

	explicit AAttachableProjectile(const FObjectInitializer& ObjectInitializer);
	virtual void InitializeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                               const FHitResult& SweepResult);

protected:
	UFUNCTION()
	virtual void OnRep_IsEnabled();

private:
	void AttachToTargetComponent();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(ReplicatedUsing=OnRep_IsEnabled, Transient)
	bool bIsEnabled;

	UPROPERTY(Replicated, Transient)
	AActor* TargetActor;

	UPROPERTY(Replicated, Transient)
	FVector InitializeLocation;

	UPROPERTY(Replicated, Transient)
	FName AttachedBone;
};
