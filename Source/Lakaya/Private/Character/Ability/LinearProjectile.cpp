// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LinearProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

const FName ALinearProjectile::SceneComponentName = FName("SceneComponent");
const FName ALinearProjectile::CollisionComponentName = FName("CollisionComponent");
const FName ALinearProjectile::StaticMeshComponentName = FName("StaticMeshComponent");

ALinearProjectile::ALinearProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = true;
	LinearVelocity = 100.f;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(CollisionComponentName);
	CollisionComponent->SetCollisionProfileName(TEXT("Trigger"));
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	CollisionComponent->SetEnableGravity(false);
	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	SetRootComponent(CollisionComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(StaticMeshComponentName);
	StaticMeshComponent->SetupAttachment(CollisionComponent);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetVisibility(true);
	StaticMeshComponent->CanCharacterStepUpOn = ECB_No;
}

void ALinearProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALinearProjectile, SummonedTime)
	DOREPLIFETIME(ALinearProjectile, SummonedLocation)
	DOREPLIFETIME(ALinearProjectile, SummonedRotation)
}

void ALinearProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ALinearProjectile::OnCollisionComponentBeginOverlap);
	StaticMeshComponent->SetVisibility(false);
}

float ALinearProjectile::GetServerTime() const
{
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void ALinearProjectile::OnSummoned()
{
	Super::OnSummoned();
	CollisionComponent->SetPhysicsLinearVelocity(GetActorForwardVector() * LinearVelocity);
	SummonedTime = GetServerTime();
	SummonedLocation = GetActorLocation();
	SummonedRotation = GetActorRotation();
	StaticMeshComponent->SetVisibility(true);
}

void ALinearProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SetActorLocation(SummonedLocation + GetActorForwardVector() * (LinearVelocity * (GetServerTime() - SummonedTime)));
}

void ALinearProjectile::OnRep_SummonedTime()
{
	const auto ProjectileState = SummonedTime > 0.f;
	SetActorTickEnabled(ProjectileState);
	StaticMeshComponent->SetVisibility(ProjectileState);
}

void ALinearProjectile::OnRep_SummonedRotation()
{
	SetActorRotation(SummonedRotation);
}

void ALinearProjectile::OnCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                         bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || OtherActor == GetInstigator() || OtherActor == this) return;
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("OnCollision"));
	SummonedTime = 0.f;
	StaticMeshComponent->SetVisibility(false);
	BroadcastOnAbilityEnded();
}
