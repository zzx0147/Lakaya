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
	static const FName StaticMeshComponentName;

	AAttachableMine();
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	virtual void SetTeam(const EPlayerTeam& Team) override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void OnActivation() override;
	virtual void OnDeactivation() override;

	virtual bool ShouldTakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                              AActor* DamageCauser);

	UFUNCTION()
	virtual void OnTriggerComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                            const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	float BaseHealth;

	UPROPERTY(EditAnywhere)
	float DamageRange;

	UPROPERTY(EditAnywhere)
	float BaseDamage;

private:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* TriggerComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMeshComponent;

	float Health;
	EPlayerTeam RecentTeam;
};
