#include "Individual/IndividualStaticEnergy.h"
#include "Character/CollectorPlayerState.h"
#include "GameMode/IndividualGameMode.h"
#include "Character/InteractableCharacter.h"

AIndividualStaticEnergy::AIndividualStaticEnergy()
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

	bIsAvailable = true;
	
	bReplicates = true;
}

void AIndividualStaticEnergy::BeginPlay()
{
	Super::BeginPlay();
}

void AIndividualStaticEnergy::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AIndividualStaticEnergy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AIndividualStaticEnergy::OnLocalInteractionBegin(APawn* Caller)
{
	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
		CastedCaller->NoticeInstantInteractionLocal();
	else UE_LOG(LogActor, Error, TEXT("OnLocalInteractionBegin::Caller was not AInteractableCharacter!"));
}

void AIndividualStaticEnergy::OnServerInteractionBegin(const float& Time, APawn* Caller)
{
	if (auto CastedCaller = Cast<AInteractableCharacter>(Caller))
		CastedCaller->InitiateInteractionStart(Time, this);
}

void AIndividualStaticEnergy::OnInteractionStart(APawn* Caller)
{
	if (!bIsAvailable)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("StaticEnergy Not Available."));
		return;
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("StaticEnergy Get !"));
	
	if (Caller && Caller->GetController())
	{
		ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(Caller->GetController()->PlayerState);
		if (CollectorPlayerState)
		{
			AInteractableCharacter* Character = Cast<AInteractableCharacter>(Caller);
			if (Character == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("OnInteractionStart_Character is null."));
				return;
			}
			CollectorPlayerState->GainEnergy(1);
			// UE_LOG(LogTemp, Warning, TEXT("Player %s has gained 1 Energy"), *CollectorPlayerState->GetPlayerName());
			UE_LOG(LogTemp, Warning, TEXT("Player Total Energy: %d"), CollectorPlayerState->GetEnergy());
			bIsAvailable = false;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CollectorPlayerState is null."));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Caller or Controller."));
	}

	// TODO : N초 후 StaticEnergy bIsAvailable = true로 변경
	GetWorldTimerManager().SetTimer(AvailableTimerHandle, this, &AIndividualStaticEnergy::MakeAvailable, 3.0f, false);
}

void AIndividualStaticEnergy::MakeAvailable()
{
	bIsAvailable = true;
}