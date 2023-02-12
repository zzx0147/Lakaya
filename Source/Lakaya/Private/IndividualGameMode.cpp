#include "IndividualGameMode.h"
#include "IndividualItem.h"
#include "TestCharacter.h"

AIndividualGameMode::AIndividualGameMode()
{
	DefaultPawnClass = ATestCharacter::StaticClass();
}

void AIndividualGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}

void AIndividualGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}