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
	OutlinePostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("OutlinePostProcessComponent"));
	ClairvoyancePostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("ClairvoyancePostProcessComponent"));
	OutlinePostProcessComponent->bEnabled = true;
	ClairvoyancePostProcessComponent->bEnabled = false;
}

// Called when the game starts or when spawned
void AOutlineManager::BeginPlay()
{
	Super::BeginPlay();
}

void AOutlineManager::SetClairvoyance(const bool& bIsClairvoyance) const
{
	OutlinePostProcessComponent->bEnabled = !bIsClairvoyance;
	ClairvoyancePostProcessComponent->bEnabled = bIsClairvoyance;
}

void AOutlineManager::RegisterClairvoyance(const uint32& UniqueId, const EPlayerTeam& PlayerTeam)
{
	if(ClientTeam != PlayerTeam) return;
	if(!ActivatedClairvoyanceSet.Contains(UniqueId))
	{
		ActivatedClairvoyanceSet.Emplace(UniqueId);
		if(!ActivatedClairvoyanceSet.IsEmpty())
			SetClairvoyance(true);
	}
}

void AOutlineManager::UnRegisterClairvoyance(const uint32& UniqueId, const EPlayerTeam& PlayerTeam)
{
	if(ClientTeam != PlayerTeam) return;
	if(ActivatedClairvoyanceSet.Contains(UniqueId))
	{
		ActivatedClairvoyanceSet.Remove(UniqueId);
		if(ActivatedClairvoyanceSet.IsEmpty())
			SetClairvoyance(false);
	}
}

