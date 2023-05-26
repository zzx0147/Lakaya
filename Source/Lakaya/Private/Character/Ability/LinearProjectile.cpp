// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LinearProjectile.h"

#include "NiagaraComponent.h"
#include "Character/Ability/CoolTimedSummonAbility.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
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
	CollisionComponent->SetEnableGravity(true);
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
}

void ALinearProjectile::PerformTimerHandler()
{
	Super::PerformTimerHandler();
	// 타이머가 종료된 시점에 아직 Ready상태라면, 클라이언트를 의미하므로 투사체에 대해 물리 시뮬레이션을 시작합니다.
	if (GetInstanceState() == EAbilityInstanceState::Ready)
		SimulateProjectilePhysics(false, false);
}

void ALinearProjectile::HandleAbilityInstancePerform()
{
	Super::HandleAbilityInstancePerform();
	// Perform 상태가 된 경우, 서버에서는 물리 엔진을 통해 투사체를 던지고, 클라이언트에서는 서버로부터 전달받은 정보를 바탕으로 투사체의 위치를 시뮬레이트합니다.
	HasAuthority() ? SimulateProjectilePhysics(true, true) : SimulateProjectileMovement();
}

void ALinearProjectile::HandleAbilityInstanceEnding()
{
	Super::HandleAbilityInstanceEnding();
	if (HasAuthority()) DisableProjectilePhysics();
	else
	{
		DisableProjectileSimulation();
		SetActorLocationAndRotation(ProjectileLocation, ProjectileRotation);
	}
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
	//TODO: 이거 최초 한번만 계산해두고, 현재 시간에 따라서 사용하는 방법이 있지 않을까?
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

void ALinearProjectile::OnCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                         bool bFromSweep, const FHitResult& SweepResult)
{
	if (bFromSweep) SetActorLocation(SweepResult.Location);
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

	SetAbilityInstanceState(EAbilityInstanceState::Ending);
}

void ALinearProjectile::SimulateProjectilePhysics(const bool& CollisionQueryEnabled,
                                                  const bool& UpdateProjectileTransform)
{
	if (!GetOwningAbility())
	{
		UE_LOG(LogScript, Error, TEXT("Owning ability was nullptr!"));
		return;
	}

	static FVector Location;
	static FRotator Rotator;
	GetOwningAbility()->GetSummonLocationAndRotation(Location, Rotator);
	SetActorLocationAndRotation(Location, Rotator);
	if (UpdateProjectileTransform)
	{
		ProjectileLocation = Location;
		ProjectileRotation = Rotator;
	}

	CollisionComponent->SetCollisionEnabled(CollisionQueryEnabled
		                                        ? ECollisionEnabled::QueryAndProbe
		                                        : ECollisionEnabled::ProbeOnly);
	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->SetPhysicsLinearVelocity(Rotator.Vector() * LinearVelocity);
	StaticMeshComponent->SetVisibility(true);
	TrailNiagaraComponent->Activate();
}

void ALinearProjectile::DisableProjectilePhysics()
{
	CollisionComponent->SetSimulatePhysics(false);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetVisibility(false);
	TrailNiagaraComponent->Deactivate();
}

void ALinearProjectile::SimulateProjectileMovement()
{
	DisableProjectilePhysics();
	SetActorTickEnabled(true);
	StaticMeshComponent->SetVisibility(true);
	TrailNiagaraComponent->Activate();
}

void ALinearProjectile::DisableProjectileSimulation()
{
	SetActorTickEnabled(false);
	StaticMeshComponent->SetVisibility(false);
	TrailNiagaraComponent->Deactivate();
}
