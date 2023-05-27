// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LinearProjectile.h"

#include "NiagaraComponent.h"
#include "Character/Ability/AttachableProjectile.h"
#include "Character/Ability/CoolTimedSummonAbility.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

const FName ALinearProjectile::CollisionComponentName = FName("CollisionComponent");
const FName ALinearProjectile::StaticMeshComponentName = FName("StaticMeshComponent");
const FName ALinearProjectile::TrailNiagaraComponentName = FName("TrailNiagaraComponent");
const FName ALinearProjectile::ExplodeNiagaraComponentName = FName("ExplodeNiagaraComponentName");

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
	CollisionComponent->SetLinearDamping(0.f);
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	SetRootComponent(CollisionComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(StaticMeshComponentName);
	StaticMeshComponent->SetupAttachment(CollisionComponent);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->CanCharacterStepUpOn = ECB_No;

	TrailNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TrailNiagaraComponentName);
	TrailNiagaraComponent->SetupAttachment(CollisionComponent);
	TrailNiagaraComponent->SetAutoActivate(false);
	TrailNiagaraComponent->SetAutoDestroy(false);

	ExplodeNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(ExplodeNiagaraComponentName);
	ExplodeNiagaraComponent->SetupAttachment(CollisionComponent);
	ExplodeNiagaraComponent->SetAutoActivate(false);
	ExplodeNiagaraComponent->SetAutoDestroy(false);

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
	CollisionComponent->SetNotifyRigidBodyCollision(HasAuthority());
	CollisionComponent->OnComponentHit.AddUniqueDynamic(this, &ALinearProjectile::OnCollisionComponentHit);
	StaticMeshComponent->SetVisibility(false);
	if (!CollisionComponent->IsGravityEnabled()) ProjectilePathParams.OverrideGravityZ = -1.f;
}

void ALinearProjectile::PerformTimerHandler()
{
	Super::PerformTimerHandler();
	// 타이머가 종료된 시점에 아직 Ready상태라면, 클라이언트를 의미하므로 투사체에 대해 물리 시뮬레이션을 시작합니다.
	if (GetInstanceState() == EAbilityInstanceState::Ready)
		SimulateProjectilePhysics(false);
}

void ALinearProjectile::HandleAbilityInstancePerform()
{
	Super::HandleAbilityInstancePerform();
	// Perform 상태가 된 경우, 서버에서는 물리 엔진을 통해 투사체를 던지고, 클라이언트에서는 서버로부터 전달받은 정보를 바탕으로 투사체의 위치를 시뮬레이트합니다.
	HasAuthority() ? SimulateProjectilePhysics(true) : SimulateProjectileMovement();
}

void ALinearProjectile::HandleAbilityInstanceEnding()
{
	Super::HandleAbilityInstanceEnding();
	if (!HasAuthority())
	{
		SetActorTickEnabled(false);
		SetActorLocationAndRotation(ProjectileLocation, ProjectileRotation);
	}
	HideProjectile();
	ExplodeNiagaraComponent->Activate();
}

void ALinearProjectile::HandleAbilityInstanceCollapsed()
{
	Super::HandleAbilityInstanceCollapsed();
	ExplodeNiagaraComponent->Deactivate();
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
	const auto& [ATeamCollision, BTeamCollision] =
		TeamCollisionMap.Contains(Team) ? TeamCollisionMap[Team] : FTeamCollisionInfo();
	CollisionComponent->SetCollisionResponseToChannel(ATeamCollisionChannel, ATeamCollision ? ECR_Block : ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(BTeamCollisionChannel, BTeamCollision ? ECR_Block : ECR_Ignore);
}

void ALinearProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority()) return;
	// 이 액터를 소환한 캐릭터와는 충돌하지 않도록 합니다.
	CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);
	GetInstigator()->MoveIgnoreActorAdd(this);
}

void ALinearProjectile::OnCollisionComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                                UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                                                const FHitResult& Hit)
{
	DisableProjectilePhysics();
	ProjectileLocation = GetActorLocation();
	ProjectileRotation = GetActorRotation();

	if (BaseDamage != 0.f && DamageRange >= 0.f)
	{
		if (DamageRange > 0.f)
		{
			UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), DamageRange, nullptr, {},
			                                    GetInstigator(), GetInstigatorController());
			DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRange, 10, FColor::Red, false, 3);
		}
		else UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, GetInstigatorController(), GetInstigator(), nullptr);
	}

	if (AttachableClass)
	{
		FActorSpawnParameters Params;
		Params.Instigator = GetInstigator();
		Params.Owner = GetOwner();
		if (const auto SpawnedActor = GetWorld()->SpawnActor<AAttachableProjectile>(AttachableClass, Params))
			SpawnedActor->InitializeHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
		else UE_LOG(LogScript, Error, TEXT("Fail to AttachableActor!"));
	}

	SetAbilityInstanceState(EAbilityInstanceState::Ending);
}

void ALinearProjectile::SimulateProjectilePhysics(const bool& UpdateProjectileTransform)
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
	SetActorLocationAndRotation(Location, Rotator);
	if (UpdateProjectileTransform)
	{
		ProjectileLocation = Location;
		ProjectileRotation = Rotator;
	}

	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::ProbeOnly);
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

void ALinearProjectile::HideProjectile()
{
	StaticMeshComponent->SetVisibility(false);
	TrailNiagaraComponent->Deactivate();
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
