#include "Individual/IndividualEnergy.h"
#include "Character/CollectorPlayerState.h"
#include "GameMode/IndividualGameMode.h"
#include "Character/MenuCallingPlayerController.h"

AIndividualEnergy::AIndividualEnergy()
{
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add("Interactable");
	
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Box"));

	RootComponent = Trigger;
	Box->SetupAttachment(RootComponent);

	Trigger->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_BOX(TEXT("/Game/Dev/KDJ/Box_D50973B1.Box_D50973B1"));
	if (SM_BOX.Succeeded())
		Box->SetStaticMesh(SM_BOX.Object);
	
	Trigger->SetRelativeLocation(FVector::ZeroVector);

	bReplicates = true;
}

void AIndividualEnergy::BeginPlay()
{
	Super::BeginPlay();
}

void AIndividualEnergy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AIndividualEnergy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIndividualEnergy::InteractionStart(const float& Time, APawn* Caller)
{
}

void AIndividualEnergy::InteractionStop(const float& Time, APawn* Caller)
{
}