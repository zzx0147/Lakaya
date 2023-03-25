#include "Individual/IndividualDropEnergy.h"
#include "Character/CollectorPlayerState.h"
#include "GameMode/IndividualGameMode.h"
#include "Character/InteractableCharacter.h"
#include "Chaos/PBDRigidClustering.h"

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

	Deactivate();
	
	bReplicates = true;
}

void AIndividualDropEnergy::BeginPlay()
{
	Super::BeginPlay();
}

void AIndividualDropEnergy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIndividualDropEnergy::OnLocalInteractionBegin(APawn* Caller)
{
	// if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
	// 	CastedCaller->NoticeInstantInteractionLocal();
	// else UE_LOG(LogActor, Error, TEXT("OnLocalInteractionBegin::Caller was not AInteractableCharacter!"));
}

void AIndividualDropEnergy::OnServerInteractionBegin(const float& Time, APawn* Caller)
{
	// if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
	// 	CastedCaller->InitiateInteractionStart(Time, this);
}

void AIndividualDropEnergy::OnInteractionStart(APawn* Caller)
{
	// UE_LOG(LogTemp, Warning, TEXT("DropEnergy Interaction."));
	
	// if (Caller && Caller->GetController())
	// {
	// 	ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(Caller->GetController()->PlayerState);
	// 	if (CollectorPlayerState)
	// 	{
	// 		AInteractableCharacter* Character = Cast<AInteractableCharacter>(Caller);
	// 		if (Character == nullptr)
	// 		{
	// 			UE_LOG(LogTemp, Warning, TEXT("InteractionStart_Character is null."));
	// 			return;
	// 		}
	// 		CollectorPlayerState->GainEnergy(1);
	// 		UE_LOG(LogTemp, Warning, TEXT("Player %s has gained 1 Energy"), *CollectorPlayerState->GetPlayerName());
	// 		UE_LOG(LogTemp, Warning, TEXT("Player Total Energy: %d"), CollectorPlayerState->GetEnergy());
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("CollectorPlayerState is null."));
	// 		return;
	// 	}
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Invalid Caller or Controller."));
	// }
	//
	// Deactivate();
}

void AIndividualDropEnergy::SetDropEnergy(AController* DeadPlayer)
{
	SetActorLocation(DeadPlayer->GetCharacter()->GetActorLocation());
	UE_LOG(LogTemp, Warning, TEXT("SetDropEnergy Function()"));
}

void AIndividualDropEnergy::Activate()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	IsActive = true;
}

void AIndividualDropEnergy::Deactivate()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	IsActive = false;
}