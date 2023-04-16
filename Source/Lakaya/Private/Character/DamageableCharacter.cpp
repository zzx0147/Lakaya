// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DamageableCharacter.h"

#include "Character/StatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


float ADamageableCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	const auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= Damage;

	// 힐이 최대체력 이상으로 이뤄지는 경우 최대체력으로 맞춰줍니다.
	if (Damage < 0.f)
		if (auto& Stat = GetStatComponent())
			if (Health > Stat->GetMaximumHealth())
				Health = Stat->GetMaximumHealth();

	OnHealthChanged.Broadcast(Health);
	NotifyDamageReceive(DamageCauser->GetActorNameOrLabel(), DamageCauser->GetActorLocation(), Damage);

	if (Health <= 0.f) KillCharacter(EventInstigator, DamageCauser);
	return Damage;
}

void ADamageableCharacter::SetupCharacterServer(const FCharacterSetupData* Data)
{
	Super::SetupCharacterServer(Data);

	bIsAlive = true;
	OnAliveChanged.Broadcast(bIsAlive);

	if (auto& Stat = GetStatComponent())
	{
		Health = Stat->GetMaximumHealth();
		OnHealthChanged.Broadcast(Health);
	}
}

void ADamageableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADamageableCharacter, Health);
	DOREPLIFETIME(ADamageableCharacter, bIsAlive);
}

void ADamageableCharacter::Respawn()
{
	bIsAlive = true;
	OnAliveChanged.Broadcast(bIsAlive);

	if (auto& Stat = GetStatComponent())
	{
		Health = Stat->GetMaximumHealth();
		OnHealthChanged.Broadcast(Health);
	}

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void ADamageableCharacter::KillCharacter(AController* EventInstigator, AActor* DamageCauser)
{
	bIsAlive = false;
	OnAliveChanged.Broadcast(bIsAlive);
	GetCharacterMovement()->DisableMovement();
}

void ADamageableCharacter::OnRep_Health()
{
	OnHealthChanged.Broadcast(Health);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Health Changed : %f"), Health));
}

void ADamageableCharacter::OnRep_IsAlive()
{
	OnAliveChanged.Broadcast(bIsAlive);
}

void ADamageableCharacter::NotifyDamageReceive_Implementation(const FString& CauserName, const FVector& AttackLocation,
                                                              const float& Damage)
{
	OnDamageReceived.Broadcast(CauserName, AttackLocation, Damage);
}
