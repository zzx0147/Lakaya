// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_EVENT_TwoParams(UHealthComponent, FOnHealthChanged, const float&, const float&);

DECLARE_EVENT(UHealthComponent, FOnDead);

UCLASS(ClassGroup=(Health), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Health)
	float MaximumHealth;

public:
	// Sets default values for this component's properties
	UHealthComponent();

	// IDK it is right. should I make it private?
	/**
	 * @brief Called when Health changed
	 * @param First Current health
	 * @param Second Health difference
	 */
	FOnHealthChanged OnHealthChanged;

	/**
	 * @brief Called when Health changed to less or equals to 0. It's running after OnHealthChanged
	 */
	FOnDead OnDead;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	float Health;

	UFUNCTION()
	void DamageHandler(AActor* DamageActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
	                   AActor* DamageCauser);
};
