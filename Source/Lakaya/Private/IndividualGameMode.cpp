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

	for (int i = 0; i < 6; i++)
	{
		AActor* item = GetWorld()->SpawnActor(AIndividualItem::StaticClass());
		ItemArray.Add(item);
		// Trigger->SetRelativeLocation(FVector(400.0f, 0.0f, 0.0f));
		item->SetActorRelativeLocation(FVector(400.0f, 200.0f * i, 0.0f));
	}

	int32 Count = ItemArray.Num();
	UE_LOG(LogTemp, Warning, TEXT("%d"), Count);
}