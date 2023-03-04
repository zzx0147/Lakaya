// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageableCharacter.h"


ADamageableCharacter::ADamageableCharacter()
{
	MaximumHealth = 100.f;
}

void ADamageableCharacter::BeginPlay()
{
	Super::BeginPlay();
	Health = MaximumHealth;
	if (IsRunningDedicatedServer()) return;

	OnTakeAnyDamage.AddDynamic(this, &ADamageableCharacter::OnTakeAnyDamageCallback);
	OnKillCharacter.AddDynamic(this, &ADamageableCharacter::OnKillCharacterCallback);
}

float ADamageableCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= Damage;
	if (Health <= 0.f) KillCharacter(EventInstigator, DamageCauser);
	return Damage;
}

void ADamageableCharacter::KillCharacter(AController* EventInstigator, AActor* DamageCauser)
{
	OnKillCharacter.Broadcast(EventInstigator, DamageCauser);
}

void ADamageableCharacter::OnTakeAnyDamageCallback(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                                   AController* InstigatedBy, AActor* DamageCauser)
{
	if (!HasAuthority()) Health -= Damage;
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red,
	                                 FString::Printf(TEXT("Damaged : %f, Health : %f"), Damage, Health));
}

void ADamageableCharacter::OnKillCharacterCallback(AController* EventInstigator, AActor* DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Dead"));
}
