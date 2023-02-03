#include "IndividualGameMode.h"
#include "Lakaya/LakayaCharacter.h"

AIndividualGameMode::AIndividualGameMode()
{
	DefaultPawnClass = ALakayaCharacter::StaticClass();
}