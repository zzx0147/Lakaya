// Fill out your copyright notice in the Description page of Project Settings.


#include "ETC/OutlineManager.h"

#include "Components/BoxComponent.h"
#include "Components/PostProcessComponent.h"

// Sets default values
AOutlineManager::AOutlineManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(BoxComponent);
	ClairvoyancePostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(
		TEXT("ClairvoyancePostProcessComponent"));
	ClairvoyancePostProcessComponent->bEnabled = true;
	EnemyRenderingParameterName = FName(TEXT("bIsRenderEnemy"));
	EnemyNumberStartParameterName = FName(TEXT("EnemyNumberStart"));
}

void AOutlineManager::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ClairvoyanceDynamic = UMaterialInstanceDynamic::Create(ClairvoyanceMaterial, this);
	if (ClairvoyanceDynamic.IsValid())
		ClairvoyancePostProcessComponent->AddOrUpdateBlendable(ClairvoyanceDynamic.Get());
}

void AOutlineManager::SetClairvoyance(const bool& bIsClairvoyance)
{
	if (ClairvoyanceDynamic.IsValid())
		ClairvoyanceDynamic->SetScalarParameterValue(EnemyRenderingParameterName, bIsClairvoyance ? 1.f : 0.f);
}

void AOutlineManager::SetTeam(const EPlayerTeam& NewTeam)
{
	ClientTeam = NewTeam;
	ClairvoyanceDynamic->SetScalarParameterValue(EnemyNumberStartParameterName,
	                                             NewTeam == EPlayerTeam::Individual ? 2.f : 8.f);
}

void AOutlineManager::RegisterClairvoyance(const uint32& UniqueId, const EPlayerTeam& PlayerTeam)
{
	if (ClientTeam != PlayerTeam || ActivatedClairvoyanceSet.Contains(UniqueId)) return;
	ActivatedClairvoyanceSet.Emplace(UniqueId);
	if (!ActivatedClairvoyanceSet.IsEmpty()) SetClairvoyance(true);
}

void AOutlineManager::UnRegisterClairvoyance(const uint32& UniqueId, const EPlayerTeam& PlayerTeam)
{
	if (ClientTeam != PlayerTeam || !ActivatedClairvoyanceSet.Contains(UniqueId)) return;
	ActivatedClairvoyanceSet.Remove(UniqueId);
	if (ActivatedClairvoyanceSet.IsEmpty()) SetClairvoyance(false);
}
