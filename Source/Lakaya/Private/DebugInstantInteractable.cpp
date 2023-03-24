// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugInstantInteractable.h"

#include "InteractableCharacter.h"
#include "Components/SphereComponent.h"

ADebugInstantInteractable::ADebugInstantInteractable()
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

void ADebugInstantInteractable::OnLocalInteractionBegin(APawn* Caller)
{
	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
		CastedCaller->NoticeInstantInteractionLocal();
	else UE_LOG(LogActor, Error, TEXT("OnLocalInteractionBegin::Caller was not AInteractableCharacter!"));
}

void ADebugInstantInteractable::OnServerInteractionBegin(const float& Time, APawn* Caller)
{
	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
		CastedCaller->InitiateInteractionStart(Time, this);
}

void ADebugInstantInteractable::OnInteractionStart(APawn* Caller)
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Instant Interaction Start!"));
}
