// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	MaximumHealth = 100;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const auto Owner = GetOwner())
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageHandler);

	Health = MaximumHealth;
}

void UHealthComponent::DamageHandler(AActor* DamageActor, float Damage, const UDamageType* DamageType,
                                     AController* InstigatedBy, AActor* DamageCauser)
{
	Health -= Damage;
	OnHealthChanged.Broadcast(Health, -Damage);
	if (Health <= 0) OnDead.Broadcast();
}
