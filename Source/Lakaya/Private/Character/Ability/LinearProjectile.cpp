// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LinearProjectile.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/Ability/CoolTimedSummonAbility.h"
#include "Components/SphereComponent.h"
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
	PerformDelay = 0.2f;
	CollapseDelay = 0.1f;
	ATeamCollisionChannel = ECC_GameTraceChannel5;
	BTeamCollisionChannel = ECC_GameTraceChannel6;

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
	DOREPLIFETIME(ALinearProjectile, ProjectileLocation)
	DOREPLIFETIME(ALinearProjectile, ProjectileRotation)
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

void ALinearProjectile::PerformTimerHandler()
{
	Super::PerformTimerHandler();
	if (GetInstanceState() == EAbilityInstanceState::Ready) SimulateProjectilePhysics();
}

void ALinearProjectile::HandleAbilityInstancePerform()
{
	Super::HandleAbilityInstancePerform();
	if (HasAuthority()) SimulateProjectilePhysics();
	else
	{
		SetActorTickEnabled(true);
		StaticMeshComponent->SetVisibility(true);
		if (TrailNiagara.IsValid()) TrailNiagara->Activate();
		CollisionComponent->SetSimulatePhysics(false);
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ALinearProjectile::HandleAbilityInstanceEnding()
{
	Super::HandleAbilityInstanceEnding();
	if (!HasAuthority()) SetActorTickEnabled(false);
	CollisionComponent->SetSimulatePhysics(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (TrailNiagara.IsValid()) TrailNiagara->Deactivate();
	if (CollisionNiagaraSystem)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CollisionNiagaraSystem, ProjectileLocation);
}

void ALinearProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	static FPredictProjectilePathParams Params;
	Params.StartLocation = ProjectileLocation;
	Params.LaunchVelocity = ProjectileRotation.Vector() * LinearVelocity;
	Params.bTraceWithCollision = false;
	Params.bTraceWithChannel = false;
	Params.MaxSimTime = GetServerTime() - GetAbilityTime();
	Params.OverrideGravityZ = CollisionComponent->IsGravityEnabled() ? GetWorld()->GetGravityZ() : -1.f;
	static FPredictProjectilePathResult Result;
	UGameplayStatics::PredictProjectilePath(GetWorld(), Params, Result);
	SetActorLocation(Result.LastTraceDestination.Location);
}

void ALinearProjectile::SetTeam(const EPlayerTeam& Team)
{
	Super::SetTeam(Team);
	CollisionComponent->SetCollisionResponseToChannel(ATeamCollisionChannel,
	                                                  Team == EPlayerTeam::B || Team == EPlayerTeam::Individual
		                                                  ? ECR_Overlap
		                                                  : ECR_Ignore);
	
	CollisionComponent->SetCollisionResponseToChannel(BTeamCollisionChannel,
	                                                  Team == EPlayerTeam::A || Team == EPlayerTeam::Individual
		                                                  ? ECR_Overlap
		                                                  : ECR_Ignore);

	if (Team == EPlayerTeam::Individual)
	{
		CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);
		GetInstigator()->MoveIgnoreActorAdd(this);
	}
}

void ALinearProjectile::OnCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                         bool bFromSweep, const FHitResult& SweepResult)
{
	auto DamageCauser = GetInstigator();
	if (bFromSweep) SetActorLocation(SweepResult.Location);
	ProjectileLocation = GetActorLocation();
	ProjectileRotation = GetActorRotation();

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
		                              DamageCauser, nullptr);
	}

	SetAbilityInstanceState(EAbilityInstanceState::Ending);
}

void ALinearProjectile::SimulateProjectilePhysics()
{
	static FVector Location;
	static FRotator Rotator;
	GetOwningAbility()->GetSummonLocationAndRotation(Location, Rotator);
	SetActorLocationAndRotation(Location, Rotator);

	if (HasAuthority())
	{
		ProjectileLocation = Location;
		ProjectileRotation = Rotator;
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndProbe);
	}
	else CollisionComponent->SetCollisionEnabled(ECollisionEnabled::ProbeOnly);

	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->SetPhysicsLinearVelocity(Rotator.Vector() * LinearVelocity);
	StaticMeshComponent->SetVisibility(true);
	if (TrailNiagara.IsValid()) TrailNiagara->Activate();
}
