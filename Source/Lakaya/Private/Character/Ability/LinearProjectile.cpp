// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LinearProjectile.h"

#include "Components/SphereComponent.h"

const FName ALinearProjectile::SphereComponentName = FName(TEXT("SphereComponent"));
const FName ALinearProjectile::StaticMeshComponentName = FName(TEXT("StaticMeshComponent"));

ALinearProjectile::ALinearProjectile()
{
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(SphereComponentName);
	SphereComponent->SetEnableGravity(false);
	SphereComponent->InitSphereRadius(50.f);
	SphereComponent->SetCollisionProfileName(TEXT("Trigger"));
	SetRootComponent(SphereComponent);

	StaticMeshComponent = CreateOptionalDefaultSubobject<UStaticMeshComponent>(StaticMeshComponentName);
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetupAttachment(SphereComponent);
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ALinearProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SphereComponent->IgnoreActorWhenMoving(this, true);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ALinearProjectile::OnSphereBeginOverlap);
}

void ALinearProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (!HasAuthority()) return;
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("NotifyActorBeginOverlap!"));
}

void ALinearProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority()) return;
	SphereComponent->AddImpulse(SphereComponent->GetForwardVector() * 3000.f);
}

void ALinearProjectile::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                             const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red,TEXT("OnSphereBeginOverlap!"));
}
