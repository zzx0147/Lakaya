// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttachableProjectile.h"
#include "AttachableMine.generated.h"

UCLASS()
class LAKAYA_API AAttachableMine : public AAttachableProjectile
{
	GENERATED_BODY()

public:
	static const FName TriggerComponentName;
	static const FName ActivationNiagaraComponentName;

	explicit AAttachableMine(const FObjectInitializer& ObjectInitializer);
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	virtual void SetTeam(const EPlayerTeam& Team) override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void HandleAbilityInstanceAction() override;
	virtual void HandleAbilityInstanceEnding() override;

	virtual bool ShouldTakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                              AActor* DamageCauser);

	UFUNCTION()
	virtual void OnTriggerComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                            const FHitResult& SweepResult);

private:
	UFUNCTION(NetMulticast, Reliable)
	void NotifyExplosion(const FVector& Location);

protected:
	UPROPERTY(EditAnywhere)
	float BaseHealth;

	UPROPERTY(EditAnywhere)
	float ExplodeRange;

	UPROPERTY(EditAnywhere)
	float ExplodeDamage;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ExplosionNiagara;

private:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* TriggerComponent;

	// 지뢰가 활성화되어있는 동안 사용될 나이아가라입니다.
	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* ActivationNiagaraComponent;

	float Health;
};
