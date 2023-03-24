// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DamageableCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameMode/IndividualGameMode.h"

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
}

float ADamageableCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= Damage;
	if (Health > MaximumHealth) Health = MaximumHealth;
	if (Health <= 0.f) KillCharacter(EventInstigator, DamageCauser);
	return Damage;
}

void ADamageableCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADamageableCharacter, MaximumHealth);
	DOREPLIFETIME(ADamageableCharacter, Health);
}

void ADamageableCharacter::Respawn()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	SetActorEnableCollision(true);
	RespawnNotify();
}

void ADamageableCharacter::KillCharacter(AController* EventInstigator, AActor* DamageCauser)
{
	GetCharacterMovement()->DisableMovement();
	//TODO: 트레이스 충돌은 꺼지지만, 여전히 다른 캐릭터의 움직임을 제한하고 있습니다..
	SetActorEnableCollision(false);
	KillCharacterNotify(EventInstigator, DamageCauser);
}

void ADamageableCharacter::KillCharacterNotify_Implementation(AController* EventInstigator, AActor* DamageCauser)
{
	OnKillCharacterNotify.Broadcast(GetController(), this, EventInstigator, DamageCauser);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, TEXT("Dead"));
}

void ADamageableCharacter::OnRep_MaximumHealth()
{
	OnMaximumHealthReplicated.Broadcast(this, MaximumHealth);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red,
	                                 FString::Printf(TEXT("MaximumHealth Changed : %f"), MaximumHealth));
}

void ADamageableCharacter::OnRep_Health()
{
	OnHealthReplicated.Broadcast(this, Health);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Health Changed : %f"), Health));
}

void ADamageableCharacter::OnTakeAnyDamageCallback(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                                   AController* InstigatedBy, AActor* DamageCauser)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, FString::Printf(TEXT("Damaged : %f"), Damage));
}

void ADamageableCharacter::RespawnNotify_Implementation()
{
	OnRespawnCharacterNotify.Broadcast(this);
}