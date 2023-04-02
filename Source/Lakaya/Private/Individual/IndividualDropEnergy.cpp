#include "Individual/IndividualDropEnergy.h"
#include "Character/CollectorPlayerState.h"
#include "GameMode/IndividualGameMode.h"
#include "Character/InteractableCharacter.h"

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

	// Deactivate();
	// SetReplicates(true);
	// SetReplicateMovement(true);

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

void AIndividualDropEnergy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AIndividualDropEnergy::OnLocalInteractionBegin(APawn* Caller)
{
	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
	{
		CastedCaller->NoticeInstantInteractionLocal();
	}
	else UE_LOG(LogActor, Error, TEXT("OnLocalInteractionBegin::Caller was not AInteractableCharacter!"));
}

void AIndividualDropEnergy::OnServerInteractionBegin(const float& Time, APawn* Caller)
{
	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
	{
		CastedCaller->InitiateInteractionStart(Time, this);
	}
}

void AIndividualDropEnergy::OnInteractionStart(APawn* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("DropEnergy Interaction."));

	if (Caller && Caller->GetController())
	{
		ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(Caller->GetController()->PlayerState);
		if (CollectorPlayerState)
		{
			UE_LOG(LogTemp, Warning, TEXT("OnInteractionStart: CollectorPlayerState is not null."));
			AInteractableCharacter* Character = Cast<AInteractableCharacter>(Caller);
			if (Character == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("OnInteractionStart: Character is null."));
				return;
			}

			CollectorPlayerState->GainEnergy(1);
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("GetEnergy !"));
			UE_LOG(LogTemp, Warning, TEXT("Player %s has gained 1 Energy"), *CollectorPlayerState->GetPlayerName());
			UE_LOG(LogTemp, Warning, TEXT("Player Total Energy: %d"), CollectorPlayerState->GetEnergy());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("OnInteractionStart: CollectorPlayerState is null."));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OnInteractionStart: Invalid Caller or Controller."));
	}

	// Deactivate();
	Destroy();
}

void AIndividualDropEnergy::LocationSetDropEnergy(AController* DeadPlayer)
{
	SetActorLocation(DeadPlayer->GetCharacter()->GetActorLocation());
}

// void AIndividualDropEnergy::Activate()
// {
// 	IsActive = true;
// 	SetActorHiddenInGame(false);
// 	SetActorEnableCollision(true);
// }
//
// void AIndividualDropEnergy::Deactivate()
// {
// 	IsActive = false;
// 	SetActorHiddenInGame(true);
// 	SetActorEnableCollision(false);
// }