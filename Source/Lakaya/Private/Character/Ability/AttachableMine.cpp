// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/AttachableMine.h"

#include "Character/LakayaBaseCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

const FName AAttachableMine::TriggerComponentName = FName(TEXT("TriggerComponent"));
const FName AAttachableMine::StaticMeshComponentName = FName(TEXT("StaticMeshComponent"));

AAttachableMine::AAttachableMine()
{
	BaseHealth = 30.f;
	DamageRange = 250.f;
	BaseDamage = 100.f;

	TriggerComponent = CreateDefaultSubobject<USphereComponent>(TriggerComponentName);
	SetRootComponent(TriggerComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(StaticMeshComponentName);
	StaticMeshComponent->SetupAttachment(TriggerComponent);
}

float AAttachableMine::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                  AActor* DamageCauser)
{
	if (!ShouldTakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser)) return 0.f;
	const auto Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= Damage;
	if (Health <= 0.f) Deactivate();
	return Damage;
}

void AAttachableMine::SetTeam(const EPlayerTeam& Team)
{
	Super::SetTeam(Team);
	RecentTeam = Team;
}

void AAttachableMine::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	TriggerComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AAttachableMine::OnTriggerComponentBeginOverlap);
}

void AAttachableMine::OnActivation()
{
	Super::OnActivation();
	if (HasAuthority()) TriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Health = BaseHealth;
}

void AAttachableMine::OnDeactivation()
{
	Super::OnDeactivation();
	if (HasAuthority()) TriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Destroy();
}

bool AAttachableMine::ShouldTakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	if (!IsActivated()) return false;
	if (!EventInstigator) return true;

	// 같은 팀이면 피해를 받지 않도록 합니다.
	if (const auto PlayerState = EventInstigator->GetPlayerState<ALakayaBasePlayerState>())
		return !PlayerState->IsSameTeam(RecentTeam);

	return true;
}

void AAttachableMine::OnTriggerComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                     bool bFromSweep, const FHitResult& SweepResult)
{
	// 다른 팀 캐릭터가 범위내에 들어오면 폭발합니다.
	if (const auto LakayaCharacter = Cast<ALakayaBaseCharacter>(OtherActor);
		LakayaCharacter && !LakayaCharacter->IsSameTeam(RecentTeam))
	{
		UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, GetActorLocation(), DamageRange, nullptr, {}, this,
		                                    GetInstigatorController());
		Deactivate();
	}
}
