// Fill out your copyright notice in the Description page of Project Settings.


#include "InGamePlayerState.h"

#include "Net/UnrealNetwork.h"

void AInGamePlayerState::ApplyDamage(const float& Damage)
{
	Health -= Damage;
}

const float& AInGamePlayerState::GetHealth() const
{
	return Health;
}

void AInGamePlayerState::BeginPlay()
{
	Super::BeginPlay();

	// temp
	Health = 100;
}

void AInGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInGamePlayerState, Health);
}
