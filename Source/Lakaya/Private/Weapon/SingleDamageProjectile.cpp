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
	bReplicates = true;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(SphereComponentName);
	SphereComponent->SetSphereRadius(50.f);
	SphereComponent->SetCollisionProfileName(TEXT("Trigger"));
	SetRootComponent(SphereComponent);

	StaticMeshComponent = CreateOptionalDefaultSubobject<UStaticMeshComponent>(StaticMeshComponentName);
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetupAttachment(SphereComponent);
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(MovementComponentName);
	MovementComponent->ProjectileGravityScale = 0.f;
}

void ASingleDamageProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 이 투사체를 생성한 캐릭터를 충돌처리에서 제외합니다.
	SphereComponent->IgnoreActorWhenMoving(GetInstigator(), true);
}

void ASingleDamageProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!HasAuthority()) return;
	UGameplayStatics::ApplyDamage(OtherActor, Damage, GetInstigator()->GetController(), GetInstigator(), nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Projectile Apply Damage!"));
	OnAttackEnded.Broadcast(this);
}

void ASingleDamageProjectile::EnableProjectile()
{
	SetActorEnableCollision(true);
	MovementComponent->AddForce(GetActorForwardVector() * MovementComponent->InitialSpeed);
	if (StaticMeshComponent) StaticMeshComponent->SetVisibility(true);
}

void ASingleDamageProjectile::DisableProjectile()
{
	SetActorEnableCollision(false);
	MovementComponent->StopMovementImmediately();
	if (StaticMeshComponent) StaticMeshComponent->SetVisibility(false);
}

void ASingleDamageProjectile::SetDamage(const float& ArgDamage)
{
	Damage = ArgDamage;
}
