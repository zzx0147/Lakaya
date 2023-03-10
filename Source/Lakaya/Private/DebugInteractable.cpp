// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugInteractable.h"

#include "Components/SphereComponent.h"

ADebugInteractable::ADebugInteractable()
{
	bReplicates = true;
	Tags.Add(TEXT("Interactable"));

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereTrigger"));
	SphereComponent->SetSphereRadius(200.f);
	SphereComponent->SetCollisionProfileName(TEXT("Trigger"));
	SetRootComponent(SphereComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(SphereComponent);
}

void ADebugInteractable::InteractionStart(const float& Time, APawn* Caller)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("InteractionStart"));
}

void ADebugInteractable::InteractionStop(const float& Time, APawn* Caller)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("InteractionStop"));
}
