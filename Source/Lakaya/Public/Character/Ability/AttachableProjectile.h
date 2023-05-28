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

	bool IsActivated() const;

protected:
	UFUNCTION()
	virtual void OnRep_ActivationTime();

	// 어태치가 이뤄진 이후, 실제로 이 오브젝트의 기능이 실행되는 타이밍에 호출됩니다.
	virtual void OnActivation();

	// 이 오브젝트의 기능이 종료되고나서 호출됩니다.
	virtual void OnDeactivation();

	// 이 오브젝트를 비활성화합니다. 반드시 서버에서 호출해야 합니다.
	virtual void Deactivate();

	float GetServerTime() const;

private:
	// OnActivation의 호출을 보장합니다. Delay가 0과 같거나 작은 경우 즉시 호출됩니다.
	void SureActivation(const float& Delay);

protected:
	UPROPERTY(EditAnywhere, meta=(ClampMin=0.f))
	float ActivationDelay;

private:
	UPROPERTY(ReplicatedUsing=OnRep_ActivationTime, Transient)
	float ActivationTime;

	UPROPERTY(Replicated, Transient)
	AActor* TargetActor;

	UPROPERTY(Replicated, Transient)
	FVector RelativeLocation;

	UPROPERTY(Replicated, Transient)
	FRotator RelativeRotator;

	UPROPERTY(Replicated, Transient)
	FName AttachedBone;

	FTimerHandle ActivationTimer;
};
