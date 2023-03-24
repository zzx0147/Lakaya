#include "IndividualStaticEnergy.h"
#include "CollectorPlayerState.h"
#include "IndividualGameMode.h"
#include "InteractableCharacter.h"

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

void AIndividualStaticEnergy::InteractionStart(const float& Time, APawn* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("InteractionStart Function."));
	
	if (Caller && Caller->GetController())
	{
		ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(Caller->GetController()->PlayerState);
		if (CollectorPlayerState)
		{
			AInteractableCharacter* Character = Cast<AInteractableCharacter>(Caller);
			if (Character == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("InteractionStart_Character is null."));
				return;
			}
			// Character->OnInteractionSuccess();
			CollectorPlayerState->GainEnergy(1);
			UE_LOG(LogTemp, Warning, TEXT("Player %s has gained 1 Energy"), *CollectorPlayerState->GetPlayerName());
			UE_LOG(LogTemp, Warning, TEXT("Player Total Energy: %d"), CollectorPlayerState->GetEnergy());
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

	SpawnEnergy();
}

void AIndividualStaticEnergy::InteractionStop(const float& Time, APawn* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("InteractionStop Function."));
	AInteractableCharacter* Character = Cast<AInteractableCharacter>(Caller->GetController());
	if (Character == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionStop_Character is null."));
		return;
	}

	Character->OnInteractionCanceled();
}

void AIndividualEnergy::SpawnEnergy()
{
	UE_LOG(LogTemp, Warning, TEXT("GetEnergy !"));
	AIndividualGameMode* GameMode = Cast<AIndividualGameMode>(GetWorld()->GetAuthGameMode());
	
	if (!GameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode is null."));
		return;
	}
	else
	{
		GameMode->SpawnStaticEnergy();
		GameMode->VectorArray.Remove(StaticEnergyNumber);
	}
	
	Destroy();
}