// Fill out your copyright notice in the Description page of Project Settings.


#include "InGamePlayerState.h"

#include "Net/UnrealNetwork.h"

void AInGamePlayerState::ApplyDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType,
                                     AController* InstigatedBy, AActor* DamageCauser)
{
	if (!HasAuthority())
	{
		UE_LOG(LogSecurity, Warning, TEXT("Client trying to apply damage. there is an logic error or client cheated"));
		return;
	}
	
	Health -= Damage;
}

void AInGamePlayerState::SetupPlayerState(APawn* Pawn)
{
	if (HasAuthority()) Pawn->OnTakeAnyDamage.AddDynamic(this, &AInGamePlayerState::ApplyDamage);
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
