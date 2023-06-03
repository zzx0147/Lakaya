// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/SmokeProjectile.h"

#include "NiagaraComponent.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/SphereComponent.h"

const FName ASmokeProjectile::TriggerComponentName = FName(TEXT("TriggerComponent"));
const FName ASmokeProjectile::SmokeNiagaraComponentName = FName(TEXT("SmokeNiagaraComponent"));

ASmokeProjectile::ASmokeProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ActionDelay = 0.2f;
	bAutoEnding = bHideMeshOnEnding = false;
	SmokeDuration = 5.f;

	TriggerComponent = CreateDefaultSubobject<USphereComponent>(TriggerComponentName);
	TriggerComponent->SetupAttachment(GetMeshComponent());
	TriggerComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TriggerComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASmokeProjectile::OnTriggerComponentBeginOverlap);
	TriggerComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &ASmokeProjectile::OnTriggerComponentEndOverlap);

	SmokeNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(SmokeNiagaraComponentName);
	SmokeNiagaraComponent->SetupAttachment(GetMeshComponent());
	SmokeNiagaraComponent->SetAutoActivate(false);
	SmokeNiagaraComponent->SetAutoDestroy(false);
}

void ASmokeProjectile::OnCollisionComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                        bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnCollisionComponentBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,
	                                        SweepResult);
	if (bFromSweep) SetActorLocation(SweepResult.Location);
	UpdateProjectileTransform();
	SetAbilityInstanceState(EAbilityInstanceState::ReadyForAction);
}

void ASmokeProjectile::HandleAbilityInstanceReadyForAction()
{
	Super::HandleAbilityInstanceReadyForAction();
	if (!HasAuthority()) SetActorLocationAndRotation(ProjectileLocation, ProjectileRotation);
}

void ASmokeProjectile::HandleAbilityInstanceAction()
{
	Super::HandleAbilityInstanceAction();
	if (!HasAuthority()) SetActorLocationAndRotation(ProjectileLocation, ProjectileRotation);
	SetStateTimer(EAbilityInstanceState::Ending, SmokeDuration);
	ActivateSmoke();
}

void ASmokeProjectile::HandleActionStateExit()
{
	Super::HandleActionStateExit();
	DeactivateSmoke();
}

void ASmokeProjectile::ActionTimerHandler()
{
	Super::ActionTimerHandler();
	ActivateSmoke();
}

void ASmokeProjectile::EndingTimerHandler()
{
	Super::EndingTimerHandler();
	DeactivateSmoke();
}

void ASmokeProjectile::OnTriggerComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                      bool bFromSweep, const FHitResult& SweepResult)
{
	if (const auto Character = Cast<ALakayaBaseCharacter>(OtherActor))
	{
		//TODO: 로컬캐릭터가 연막탄 내에 존재하는 경우 모든 캐릭터의 아웃라인을 끕니다.
		//TODO: 로컬캐릭터는 연막탄 내에 없고, 다른 캐릭터들만 연막탄 내에 존재하는 경우 그 캐릭터들만 아웃라인을 끕니다.
	}
}

void ASmokeProjectile::OnTriggerComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const auto Character = Cast<ALakayaBaseCharacter>(OtherActor))
	{
		//TODO: 캐릭터의 아웃라인을 켭니다.
	}
}

void ASmokeProjectile::ActivateSmoke()
{
	if (TriggerComponent->GetCollisionEnabled() == ECollisionEnabled::QueryOnly) return;
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SmokeNiagaraComponent->Activate(true);
}

void ASmokeProjectile::DeactivateSmoke()
{
	if (TriggerComponent->GetCollisionEnabled() == ECollisionEnabled::NoCollision) return;
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SmokeNiagaraComponent->Deactivate();
}
