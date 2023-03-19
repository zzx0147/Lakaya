#include "IndividualDropEnergy.h"
#include "Components/SphereComponent.h"

AIndividualDropEnergy::AIndividualDropEnergy()
{
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add("Interactable");

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));

	RootComponent = Trigger;
	Sphere->SetupAttachment(RootComponent);

	Trigger->SetSphereRadius(50.0f, true);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SM_BOX.Succeeded())
		Sphere->SetStaticMesh(SM_BOX.Object);
	
	Trigger->SetRelativeLocation(FVector::ZeroVector);

	bReplicates = true;
}

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
}

void AIndividualDropEnergy::InteractionStop(const float& Time, APawn* Caller)
{
}