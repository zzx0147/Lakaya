#include "IndividualDropEnergy.h"

void AIndividualDropEnergy::BeginPlay()
{
	Super::BeginPlay();
}

void AIndividualDropEnergy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AIndividualDropEnergy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIndividualDropEnergy::InteractionStart(const float& Time, APawn* Caller)
{
	// IInteractable::InteractionStart(Time, Caller);
}

void AIndividualDropEnergy::InteractionStop(const float& Time, APawn* Caller)
{
	// IInteractable::InteractionStop(Time, Caller);
}