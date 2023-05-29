// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LinearProjectile.h"
#include "AttachableProjectile.generated.h"

UCLASS()
class LAKAYA_API AAttachableProjectile : public ALinearProjectile
{
	GENERATED_BODY()

public:
	explicit AAttachableProjectile(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void OnCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                              const FHitResult& SweepResult) override;

	virtual void HandleAbilityInstanceReadyForAction() override;
	virtual void HandleAbilityInstanceAction() override;
	virtual void HandleAbilityInstanceCollapsed() override;

private:
	UPROPERTY(Replicated, Transient)
	AActor* TargetActor;

	UPROPERTY(Replicated, Transient)
	FName AttachedBone;
};
