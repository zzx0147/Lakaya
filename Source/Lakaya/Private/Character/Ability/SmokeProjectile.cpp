// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/SmokeProjectile.h"

#include "NiagaraComponent.h"
#include "Character/LakayaBaseCharacter.h"
#include "Components/SphereComponent.h"

const FName ASmokeProjectile::TriggerComponentName = FName(TEXT("TriggerComponent"));
const FName ASmokeProjectile::SmokeNiagaraComponentName = FName(TEXT("SmokeNiagaraComponent"));
const FName ASmokeProjectile::SmokeMeshComponentName = FName(TEXT("SmokeMeshComponent"));

ASmokeProjectile::ASmokeProjectile(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ActionDelay = 0.2f;
	bAutoEnding = bHideMeshOnEnding = false;
	SmokeDuration = 5.f;

	TriggerComponent = CreateDefaultSubobject<USphereComponent>(TriggerComponentName);
	TriggerComponent->SetupAttachment(GetMeshComponent());
	TriggerComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerComponent->SetCollisionResponseToChannel(IndividualCollisionChannel, ECR_Overlap);
	TriggerComponent->SetCollisionResponseToChannel(ATeamCollisionChannel, ECR_Overlap);
	TriggerComponent->SetCollisionResponseToChannel(BTeamCollisionChannel, ECR_Overlap);
	TriggerComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ASmokeProjectile::OnTriggerComponentBeginOverlap);
	TriggerComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &ASmokeProjectile::OnTriggerComponentEndOverlap);

	SmokeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(SmokeMeshComponentName);
	SmokeMeshComponent->SetupAttachment(TriggerComponent);
	SmokeMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SmokeMeshComponent->SetRenderCustomDepth(true);
	SmokeMeshComponent->SetCustomDepthStencilValue(255);
	SmokeMeshComponent->SetCustomDepthStencilWriteMask(ERendererStencilMask::ERSM_128);

	SmokeNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(SmokeNiagaraComponentName);
	SmokeNiagaraComponent->SetupAttachment(SmokeMeshComponent);
	SmokeNiagaraComponent->SetAutoActivate(false);
	SmokeNiagaraComponent->SetAutoDestroy(false);
}

void ASmokeProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SmokeNiagaraComponent->Deactivate();
	SmokeMeshComponent->SetVisibility(false);
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		Character->EnableClairvoyance();
		//GEngine->AddOnScreenDebugMessage(-1,3,FColor::White,TEXT("Smoke enter"));
	}
}

void ASmokeProjectile::OnTriggerComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const auto Character = Cast<ALakayaBaseCharacter>(OtherActor))
	{
		Character->DisableClairvoyance();
		//GEngine->AddOnScreenDebugMessage(-1,3,FColor::White,TEXT("Smoke exit"));
	}
}

void ASmokeProjectile::ActivateSmoke()
{
	if (TriggerComponent->GetCollisionEnabled() == ECollisionEnabled::QueryOnly) return;
	SmokeNiagaraComponent->Activate();
	SmokeMeshComponent->SetVisibility(true);
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ASmokeProjectile::DeactivateSmoke()
{
	if (TriggerComponent->GetCollisionEnabled() == ECollisionEnabled::NoCollision) return;
	SmokeNiagaraComponent->Deactivate();
	SmokeMeshComponent->SetVisibility(false);
	TriggerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
