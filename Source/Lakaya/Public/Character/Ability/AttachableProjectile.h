// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Occupation/PlayerTeam.h"
#include "AttachableProjectile.generated.h"

UCLASS()
class LAKAYA_API AAttachableProjectile : public AActor
{
	GENERATED_BODY()

public:
	AAttachableProjectile();
	virtual void InitializeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                               const FHitResult& SweepResult, const FVector& Velocity);

	virtual void SetTeam(const EPlayerTeam& Team) { return; }

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
