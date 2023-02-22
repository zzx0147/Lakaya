// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPlayerState.h"

#include "Net/UnrealNetwork.h"

void AHealthPlayerState::ApplyDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType,
                                     AController* InstigatedBy, AActor* DamageCauser)
{
	if (!HasAuthority())
	{
		UE_LOG(LogSecurity, Warning, TEXT("Client trying to apply damage. there is an logic error or client cheated"));
		return;
	}
	
	Health -= Damage;
}

void AHealthPlayerState::SetupPlayerState(APawn* Pawn)
{
	if (HasAuthority()) Pawn->OnTakeAnyDamage.AddDynamic(this, &AHealthPlayerState::ApplyDamage);
}

const float& AHealthPlayerState::GetHealth() const
{
	return Health;
}

void AHealthPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// temp
	Health = 100;
}

void AHealthPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHealthPlayerState, Health);
}
