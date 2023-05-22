// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LinearProjectile.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

const FName ALinearProjectile::SceneComponentName = FName("SceneComponent");
const FName ALinearProjectile::CollisionComponentName = FName("CollisionComponent");
const FName ALinearProjectile::StaticMeshComponentName = FName("StaticMeshComponent");

ALinearProjectile::ALinearProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = true;
	LinearVelocity = 1000.f;
	BaseDamage = 20.f;
	DamageRange = 100.f;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(CollisionComponentName);
	CollisionComponent->SetCollisionProfileName(TEXT("DefaultProjectile"));
	CollisionComponent->SetEnableGravity(true);
	CollisionComponent->SetLinearDamping(0.f);
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	SetRootComponent(CollisionComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(StaticMeshComponentName);
	StaticMeshComponent->SetupAttachment(CollisionComponent);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	if (TrailNiagaraSystem)
	{
		TrailNiagara = UNiagaraFunctionLibrary::SpawnSystemAttached(TrailNiagaraSystem, RootComponent, NAME_None,
		                                                            FVector::ZeroVector, FRotator::ZeroRotator,
		                                                            EAttachLocation::SnapToTarget, false, false);
	}
}

float ALinearProjectile::GetServerTime() const
{
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void ALinearProjectile::OnSummoned()
{
	Super::OnSummoned();
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndProbe);
	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->SetPhysicsLinearVelocity(GetActorRotation().Vector() * LinearVelocity);
	SummonedTime = GetServerTime();
	SummonedLocation = GetActorLocation();
	SummonedRotation = GetActorRotation();
	StaticMeshComponent->SetVisibility(true);
	if (TrailNiagara.IsValid()) TrailNiagara->Activate();
}

void ALinearProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	static FPredictProjectilePathParams Params;
	Params.StartLocation = SummonedLocation;
	Params.LaunchVelocity = SummonedRotation.Vector() * LinearVelocity;
	Params.bTraceWithCollision = false;
	Params.bTraceWithChannel = false;
	Params.MaxSimTime = GetServerTime() - SummonedTime;
	Params.OverrideGravityZ = CollisionComponent->IsGravityEnabled() ? GetWorld()->GetGravityZ() : -1.f;
	static FPredictProjectilePathResult Result;
	UGameplayStatics::PredictProjectilePath(GetWorld(), Params, Result);
	SetActorLocation(Result.LastTraceDestination.Location);
}

void ALinearProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);
	GetInstigator()->MoveIgnoreActorAdd(this);
}

void ALinearProjectile::OnRep_SummonedTime()
{
	const auto ProjectileState = SummonedTime > 0.f;
	SetActorTickEnabled(ProjectileState);
	StaticMeshComponent->SetVisibility(ProjectileState);
	if (TrailNiagara.IsValid()) ProjectileState ? TrailNiagara->Activate() : TrailNiagara->Deactivate();
	
	if (!ProjectileState)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CollisionNiagaraSystem, GetActorLocation());
}

void ALinearProjectile::OnRep_SummonedLocation()
{
	SetActorLocation(SummonedLocation);
}

void ALinearProjectile::OnRep_SummonedRotation()
{
	SetActorRotation(SummonedRotation);
}

void ALinearProjectile::OnCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                         bool bFromSweep, const FHitResult& SweepResult)
{
	auto DamageCauser = GetInstigator();
	if (!HasAuthority() || OtherActor == DamageCauser || OtherActor == this) return;
	
	if (bFromSweep) SetActorLocation(SweepResult.Location);
	
	if (DamageRange > 0.f)
	{
		UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), DamageRange, nullptr,
		                                    {this, DamageCauser}, DamageCauser,
		                                    DamageCauser ? DamageCauser->GetController() : nullptr);
		DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRange, 10, FColor::Red, false, 3);
	}
	else
	{
		UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, DamageCauser ? DamageCauser->GetController() : nullptr,
		                              DamageCauser,
		                              nullptr);
	}

	if (TrailNiagara.IsValid()) TrailNiagara->Deactivate();
	
	if (CollisionNiagaraSystem)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CollisionNiagaraSystem, GetActorLocation());
	
	CollisionComponent->SetSimulatePhysics(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetVisibility(false);
	SummonedTime = 0.f;
	SummonedLocation = GetActorLocation();
	BroadcastOnAbilityEnded();
}
