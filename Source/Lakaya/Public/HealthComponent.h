// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, const float&, ChangedHealth, const float&,
                                             Delta);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeadSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Health)
	float MaximumHealth;

public:
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = Events)
	FOnDeadSignature OnDead;

private:
	float Health;

	void DamageHandler(AActor* DamageActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
	                   AActor* DamageCauser);
};
