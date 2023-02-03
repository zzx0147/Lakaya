#include "IndividualGameMode.h"
#include "TestCharacter.h"

AIndividualGameMode::AIndividualGameMode()
{
	DefaultPawnClass = ATestCharacter::StaticClass();
}