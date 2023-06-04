// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LinearProjectile.h"
#include "SmokeProjectile.generated.h"

UCLASS()
class LAKAYA_API ASmokeProjectile : public ALinearProjectile
{
	GENERATED_BODY()

public:
	static const FName TriggerComponentName;
	static const FName SmokeNiagaraComponentName;

	explicit ASmokeProjectile(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void OnCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                              const FHitResult& SweepResult) override;
	virtual void HandleAbilityInstanceReadyForAction() override;
	virtual void HandleAbilityInstanceAction() override;
	virtual void HandleActionStateExit() override;
	virtual void ActionTimerHandler() override;
	virtual void EndingTimerHandler() override;

	UFUNCTION()
	virtual void OnTriggerComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                            const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnTriggerComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	void ActivateSmoke();
	void DeactivateSmoke();

protected:
	// 연막탄의 지속시간을 지정합니다.
	UPROPERTY(EditAnywhere)
	float SmokeDuration;

private:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* TriggerComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* SmokeNiagaraComponent;

	FTimerHandle EndingTimer;
};
