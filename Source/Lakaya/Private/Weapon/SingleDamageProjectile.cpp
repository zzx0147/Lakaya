// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/SingleDamageProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

const FName ASingleDamageProjectile::SphereComponentName = FName(TEXT("SphereComponent"));
const FName ASingleDamageProjectile::StaticMeshComponentName = FName(TEXT("StaticMeshComponent"));
const FName ASingleDamageProjectile::MovementComponentName = FName(TEXT("MovementComponent"));

ASingleDamageProjectile::ASingleDamageProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(SphereComponentName);
	SphereComponent->SetSphereRadius(100.f);
	SetRootComponent(SphereComponent);

	StaticMeshComponent = CreateOptionalDefaultSubobject<UStaticMeshComponent>(StaticMeshComponentName);
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetupAttachment(SphereComponent);
	}

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(MovementComponentName);
}

void ASingleDamageProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!HasAuthority()) return;
	UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigator<AController>(), GetOwner(), nullptr);
}

void ASingleDamageProjectile::SetDamage(const float& ArgDamage)
{
	Damage = ArgDamage;
}
