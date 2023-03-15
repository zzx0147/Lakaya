#include "IndividualStaticEnergy.h"
#include "CollectorPlayerState.h"
#include "IndividualGameMode.h"

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
	
	GetItem();

	if (Caller && Caller->GetController())
	{
		ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(Caller->GetController()->PlayerState);
		if (CollectorPlayerState)
		{
			CollectorPlayerState->GainEnergy(1);
			UE_LOG(LogTemp, Warning, TEXT("Player %s has gained 1 Energy"), *CollectorPlayerState->GetPlayerName());
			UE_LOG(LogTemp, Warning, TEXT("Player Total Energy: %d"), CollectorPlayerState->GetEnergy());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CollectorPlayerState is null."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Caller or Controller."));
	}
}

void AIndividualStaticEnergy::InteractionStop(const float& Time, APawn* Caller)
{
	UE_LOG(LogTemp, Warning, TEXT("InteractionStop Function."));
}

void AIndividualEnergy::GetItem()
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
