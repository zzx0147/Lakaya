// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LinearProjectile.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/Ability/CoolTimedSummonAbility.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

const FName ALinearProjectile::CollisionComponentName = FName("CollisionComponent");
const FName ALinearProjectile::StaticMeshComponentName = FName("StaticMeshComponent");
const FName ALinearProjectile::TrailNiagaraComponentName = FName("TrailNiagaraComponent");

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
	bHideMeshOnEnding = bAutoEnding = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(CollisionComponentName);
	CollisionComponent->SetCollisionProfileName(TEXT("DefaultProjectile"));
	CollisionComponent->SetLinearDamping(0.f);
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	SetRootComponent(CollisionComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(StaticMeshComponentName);
	StaticMeshComponent->SetupAttachment(CollisionComponent);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->CanCharacterStepUpOn = ECB_No;

	TrailNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TrailNiagaraComponentName);
	TrailNiagaraComponent->SetupAttachment(StaticMeshComponent);
	TrailNiagaraComponent->SetAutoActivate(false);
	TrailNiagaraComponent->SetAutoDestroy(false);

	ProjectilePathParams.MaxSimTime = 5.f;
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
	CollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(
		this, &ALinearProjectile::OnCollisionComponentBeginOverlap);
	StaticMeshComponent->SetVisibility(false);
	if (!CollisionComponent->IsGravityEnabled()) ProjectilePathParams.OverrideGravityZ = -1.f;
}

void ALinearProjectile::PerformTimerHandler()
{
	Super::PerformTimerHandler();
	// 타이머가 종료된 시점에 아직 Ready상태라면 투사체에 대해 물리 시뮬레이션을 시작합니다.
	if (GetInstanceState() == EAbilityInstanceState::Ready) SimulateProjectilePhysics();
}

void ALinearProjectile::HandleAbilityInstanceReady()
{
	Super::HandleAbilityInstanceReady();
	DisableProjectileSimulation();
	StaticMeshComponent->SetVisibility(false);
}

void ALinearProjectile::HandleAbilityInstancePerform()
{
	Super::HandleAbilityInstancePerform();
	// Perform 상태가 된 경우, 서버에서는 물리 엔진을 통해 투사체를 던지고, 클라이언트에서는 서버로부터 전달받은 정보를 바탕으로 투사체의 위치를 시뮬레이트합니다.
	if (HasAuthority())
	{
		SimulateProjectilePhysics(true);
		UpdateProjectileTransform();
	}
	else SimulateProjectileMovement();
}

void ALinearProjectile::DisableProjectileSimulation()
{
	TrailNiagaraComponent->Deactivate();
	SetActorTickEnabled(false);
}

void ALinearProjectile::HandleAbilityInstanceEnding()
{
	Super::HandleAbilityInstanceEnding();
	DisableProjectileSimulation();
	if (bAutoEnding)
	{
		if (!HasAuthority()) SetActorLocationAndRotation(ProjectileLocation, ProjectileRotation);
		if (bHideMeshOnEnding) StaticMeshComponent->SetVisibility(false);
	}
}

void ALinearProjectile::HandleAbilityInstanceReadyForAction()
{
	Super::HandleAbilityInstanceReadyForAction();
	DisableProjectileSimulation();
}

void ALinearProjectile::HandleAbilityInstanceAction()
{
	Super::HandleAbilityInstanceAction();
	DisableProjectileSimulation();
}

void ALinearProjectile::HandleAbilityInstanceCollapsed()
{
	Super::HandleAbilityInstanceCollapsed();
	DisableProjectileSimulation();
	StaticMeshComponent->SetVisibility(false);
}

void ALinearProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float CurrentTime = GetServerTime();

	// 투사체의 최초 위치의 시간으로부터 얼마나 흘렀는지 구합니다.
	float TimeDiff = CurrentTime - RecentPathCalculateTime;

	// 계산되어있는 투사체 경로의 마지막 위치의 시간을 지났다면 추가적으로 경로를 계산합니다.
	while (TimeDiff >= ProjectilePathResult.LastTraceDestination.Time)
	{
		RecalculateProjectilePath();
		TimeDiff = CurrentTime - RecentPathCalculateTime;
	}
	ProjectilePathResult.PathData.Heapify(CustomPointDataPredicate);

	// 가장 먼저 보이는 경과시간값보다 큰 데이터를 찾고, 그 이전 데이터는 저장해둡니다.
	auto TopData = &ProjectilePathResult.PathData.HeapTop();
	while (TopData->Time <= TimeDiff)
	{
		ProjectilePathResult.PathData.HeapPop(RecentPointData, CustomPointDataPredicate, false);
		TopData = &ProjectilePathResult.PathData.HeapTop();
	}

	// 두 데이터를 보간하여 현재 시간에 맞는 위치로 이동시킵니다.
	SetActorLocation(FMath::Lerp(RecentPointData.Location, TopData->Location,
	                             UKismetMathLibrary::NormalizeToRange(TimeDiff, RecentPointData.Time, TopData->Time)));
}

void ALinearProjectile::SetTeam(const EPlayerTeam& Team)
{
	Super::SetTeam(Team);
	if (RecentTeam == Team) return;
	RecentTeam = Team;
	const auto& [ATeamCollision, BTeamCollision] =
		TeamCollisionMap.Contains(Team) ? TeamCollisionMap[Team] : FTeamCollisionInfo();
	CollisionComponent->SetCollisionResponseToChannel(ATeamCollisionChannel, ATeamCollision ? ECR_Overlap : ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(BTeamCollisionChannel, BTeamCollision ? ECR_Overlap : ECR_Ignore);
}

void ALinearProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority()) return;
	// 이 액터를 소환한 캐릭터와는 충돌하지 않도록 합니다.
	CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);
	GetInstigator()->MoveIgnoreActorAdd(this);
}

void ALinearProjectile::UpdateProjectileTransform()
{
	ProjectileLocation = GetActorLocation();
	ProjectileRotation = GetActorRotation();
}

void ALinearProjectile::OnCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                         bool bFromSweep, const FHitResult& SweepResult)
{
	const auto Velocity = CollisionComponent->GetPhysicsLinearVelocity();
	if (bAutoEnding) DisableProjectilePhysics();

	if (BaseDamage != 0.f)
	{
		if (DamageRange > 0.f)
		{
			UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), DamageRange, nullptr, {},
			                                    GetInstigator(), GetInstigatorController());
			DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRange, 10, FColor::Red, false, 3);
			NotifyExplosion(GetActorLocation(), Velocity);
		}
		else if (DamageRange == 0.f)
		{
			UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, GetInstigatorController(), GetInstigator(), nullptr);
			NotifyExplosion(GetActorLocation(), Velocity);
		}
	}

	if (bAutoEnding)
	{
		UpdateProjectileTransform();
		SetAbilityInstanceState(EAbilityInstanceState::Ending);
	}
}

void ALinearProjectile::SimulateProjectilePhysics(const bool& UsingQuery)
{
	if (!GetOwningAbility())
	{
		UE_LOG(LogScript, Error, TEXT("Owning ability was nullptr!"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 3, HasAuthority() ? FColor::White : FColor::Green,
	                                 TEXT("Start projectile physics"));

	static FVector Location;
	static FRotator Rotator;
	GetOwningAbility()->GetSummonLocationAndRotation(Location, Rotator);
	CollisionComponent->SetWorldLocationAndRotationNoPhysics(Location, Rotator);
	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->SetCollisionEnabled(
		UsingQuery ? ECollisionEnabled::QueryAndProbe : ECollisionEnabled::ProbeOnly);
	CollisionComponent->SetPhysicsLinearVelocity(Rotator.Vector() * LinearVelocity);
	ShowProjectile();
}

void ALinearProjectile::DisableProjectilePhysics()
{
	CollisionComponent->SetSimulatePhysics(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALinearProjectile::SimulateProjectileMovement()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, HasAuthority() ? FColor::White : FColor::Green,
	                                 TEXT("Start projectile simulate"));
	DisableProjectilePhysics();
	CalculateProjectilePath(ProjectileLocation, ProjectileRotation);
	ProjectilePathResult.PathData.Heapify(CustomPointDataPredicate);
	RecentPointData = ProjectilePathResult.PathData.HeapTop();
	SetActorTickEnabled(true);
	ShowProjectile();
}

void ALinearProjectile::ShowProjectile()
{
	StaticMeshComponent->SetVisibility(true);
	TrailNiagaraComponent->Activate();
}

void ALinearProjectile::CalculateProjectilePath(const FVector& Location, const FRotator& Rotator)
{
	RecentPathCalculateTime = GetAbilityTime();
	ProjectilePathParams.StartLocation = Location;
	ProjectilePathParams.LaunchVelocity = Rotator.Vector() * LinearVelocity;
	UGameplayStatics::PredictProjectilePath(GetWorld(), ProjectilePathParams, ProjectilePathResult);
}

void ALinearProjectile::RecalculateProjectilePath()
{
	RecentPathCalculateTime += ProjectilePathResult.LastTraceDestination.Time;
	RecentPointData = ProjectilePathResult.LastTraceDestination;
	ProjectilePathParams.StartLocation = RecentPointData.Location;
	ProjectilePathParams.LaunchVelocity = RecentPointData.Velocity;
	UGameplayStatics::PredictProjectilePath(GetWorld(), ProjectilePathParams, ProjectilePathResult);
}

bool ALinearProjectile::CustomPointDataPredicate(const FPredictProjectilePathPointData& First,
                                                 const FPredictProjectilePathPointData& Second)
{
	return First.Time < Second.Time;
}

void ALinearProjectile::NotifyExplosion_Implementation(const FVector& Location, const FVector& Direction)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionNiagara, Location, (-Direction).Rotation());
}
