// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/AttachableMine.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/LakayaBaseCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

const FName AAttachableMine::TriggerComponentName = FName(TEXT("TriggerComponent"));
const FName AAttachableMine::ActivationNiagaraComponentName = FName(TEXT("ActivationNiagaraComponent"));
const FName AAttachableMine::DamageComponentName = FName(TEXT("DamageComponent"));

AAttachableMine::AAttachableMine(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BaseHealth = 30.f;
	ExplodeRange = 250.f;
	ExplodeDamage = 100.f;
	bInstigatorExplode = bAllyExplode = false;
	bEnemyExplode = true;
	ExplosionOffset = 40.f;

	TriggerComponent = CreateDefaultSubobject<USphereComponent>(TriggerComponentName);
	TriggerComponent->SetupAttachment(RootComponent);
	TriggerComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TriggerComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AAttachableMine::OnTriggerComponentBeginOverlap);

	ActivationNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(ActivationNiagaraComponentName);
	ActivationNiagaraComponent->SetupAttachment(GetMeshComponent());
	ActivationNiagaraComponent->SetAutoActivate(false);
	ActivationNiagaraComponent->SetAutoDestroy(false);

	DamageComponent = CreateDefaultSubobject<USphereComponent>(DamageComponentName);
	DamageComponent->SetupAttachment(GetMeshComponent());
	DamageComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
}

float AAttachableMine::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                  AActor* DamageCauser)
{
	if (!ShouldTakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser)) return 0.f;
	const auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= Damage;
	if (Health <= 0.f) SetAbilityInstanceState(EAbilityInstanceState::Ending);
	return Damage;
}

void AAttachableMine::SetTeam(const ETeam& Team)
{
	Super::SetTeam(Team);
	if (HasAuthority()) SetTeamCollisionResponse(TriggerComponent, Team, bAllyExplode, bEnemyExplode);
}

void AAttachableMine::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority() && !bInstigatorExplode) SetIgnoreInstigator(TriggerComponent);
}

void AAttachableMine::HandleAbilityInstanceAction()
{
	Super::HandleAbilityInstanceAction();
	ActivationNiagaraComponent->Activate();
	if (!HasAuthority()) return;
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Health = BaseHealth;
}

void AAttachableMine::HandleActionStateExit()
{
	Super::HandleActionStateExit();
	ActivationNiagaraComponent->Deactivate();
	if (!HasAuthority()) return;
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DamageComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}

bool AAttachableMine::ShouldTakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	if (GetInstanceState() != EAbilityInstanceState::Action || !HasAuthority()) return false;
	if (!EventInstigator) return true;

	// 같은 팀이면 피해를 받지 않도록 합니다.
	if (const auto PlayerState = EventInstigator->GetPlayerState<ALakayaBasePlayerState>())
		return !PlayerState->IsSameTeam(GetTeam());

	return true;
}

void AAttachableMine::OnTriggerComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                     bool bFromSweep, const FHitResult& SweepResult)
{
	const auto Location = GetActorLocation();
	const auto ExplodeLocation = Location + GetActorForwardVector() * ExplosionOffset;
	UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplodeDamage,
	                                    ExplodeLocation, ExplodeRange,
	                                    nullptr, {}, this, GetInstigatorController());

	NotifyExplosion(Location);
	SetAbilityInstanceState(EAbilityInstanceState::Ending);

	// DrawDebugLine(GetWorld(), Location, ExplodeLocation, FColor::Green, false, 5.f);
	// DrawDebugSphere(GetWorld(), ExplodeLocation, ExplodeRange, 10, FColor::Red, false, 5.f);
}

void AAttachableMine::NotifyExplosion_Implementation(const FVector& Location)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionNiagara, Location);
}
