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

AAttachableMine::AAttachableMine(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BaseHealth = 30.f;
	ExplodeRange = 250.f;
	ExplodeDamage = 100.f;

	GetMeshComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMeshComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	TriggerComponent = CreateDefaultSubobject<USphereComponent>(TriggerComponentName);
	TriggerComponent->SetupAttachment(RootComponent);
	TriggerComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ActivationNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(ActivationNiagaraComponentName);
	ActivationNiagaraComponent->SetupAttachment(GetMeshComponent());
	ActivationNiagaraComponent->SetAutoActivate(false);
	ActivationNiagaraComponent->SetAutoDestroy(false);
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

void AAttachableMine::SetTeam(const EPlayerTeam& Team)
{
	if (GetTeam() == Team) return;
	Super::SetTeam(Team);
	if (!HasAuthority()) return;
	const auto& [ATeamCollision, BTeamCollision] =
		TeamCollisionMap.Contains(Team) ? TeamCollisionMap[Team] : FTeamCollisionInfo();
	TriggerComponent->SetCollisionResponseToChannel(ATeamCollisionChannel, ATeamCollision ? ECR_Overlap : ECR_Ignore);
	TriggerComponent->SetCollisionResponseToChannel(BTeamCollisionChannel, BTeamCollision ? ECR_Overlap : ECR_Ignore);
}

void AAttachableMine::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	TriggerComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AAttachableMine::OnTriggerComponentBeginOverlap);
}

void AAttachableMine::HandleAbilityInstanceAction()
{
	Super::HandleAbilityInstanceAction();
	ActivationNiagaraComponent->Activate();
	if (!HasAuthority()) return;
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Health = BaseHealth;
}

void AAttachableMine::HandleAbilityInstanceEnding()
{
	Super::HandleAbilityInstanceEnding();
	ActivationNiagaraComponent->Deactivate();
	if (HasAuthority()) TriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	UGameplayStatics::ApplyRadialDamage(GetWorld(), ExplodeDamage, GetActorLocation(), ExplodeRange, nullptr, {}, this,
	                                    GetInstigatorController());
	NotifyExplosion(GetActorLocation());
	SetAbilityInstanceState(EAbilityInstanceState::Ending);
}

void AAttachableMine::NotifyExplosion_Implementation(const FVector& Location)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionNiagara, Location);
}
