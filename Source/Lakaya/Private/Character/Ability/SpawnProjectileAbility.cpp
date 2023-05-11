// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/SpawnProjectileAbility.h"

#include "Net/UnrealNetwork.h"
#include "Weapon/SingleDamageProjectile.h"

USpawnProjectileAbility::USpawnProjectileAbility()
{
	CoolTime = 5.f;
	SpawnDistance = 50.f;
}

void USpawnProjectileAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USpawnProjectileAbility, EnableTime);
}


void USpawnProjectileAbility::AbilityStart()
{
	if (EnableTime <= GetServerTime()) Super::AbilityStart();
}

void USpawnProjectileAbility::RequestStart_Implementation(const float& RequestTime)
{
	Super::RequestStart_Implementation(RequestTime);
	if (EnableTime > GetServerTime()) return;

	const auto SpawnLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * SpawnDistance;
	const auto SpawnRotation = GetOwner()->GetActorRotation();
	FActorSpawnParameters Params;
	Params.Owner = GetTypedOuter<AController>();
	Params.Instigator = GetTypedOuter<APawn>();

	if (const auto Projectile = Cast<ASingleDamageProjectile>(
		GetWorld()->SpawnActor(ProjectileClass, &SpawnLocation, &SpawnRotation, Params)))
		Projectile->SetDamage(BaseDamage);

	EnableTime = GetServerTime() + CoolTime;
	OnEnableTimeChanged.Broadcast(EnableTime);
}

void USpawnProjectileAbility::OnRep_EnableTime()
{
	OnEnableTimeChanged.Broadcast(EnableTime);
}
