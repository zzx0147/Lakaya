// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovableCharacter.h"
#include "DamageableCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FKillCharacterSignature, AController*, EventInstigator, AActor*,
                                             DamageCauser);

UCLASS()
class LAKAYA_API ADamageableCharacter : public AMovableCharacter
{
	GENERATED_BODY()

public:
	ADamageableCharacter();
	
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	
	UFUNCTION()
	virtual void OnTakeAnyDamageCallback(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	                                     AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	virtual void OnKillCharacterCallback(AController* EventInstigator, AActor* DamageCauser);
	
	virtual void KillCharacter(AController* EventInstigator, AActor* DamageCauser);

	FKillCharacterSignature OnKillCharacter;

private:
	UPROPERTY(EditAnywhere, Category=Health)
	float MaximumHealth;

	float Health;
};
