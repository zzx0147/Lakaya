#include "Interactable/Interactable.h"

AInteractable::AInteractable(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Tags.Add("Interactable");

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Sphere"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	SetRootComponent(Trigger);
	Mesh->SetupAttachment(RootComponent);

	Trigger->InitSphereRadius(600.0f);
	Trigger->SetCollisionProfileName(TEXT("Interactable"));
}