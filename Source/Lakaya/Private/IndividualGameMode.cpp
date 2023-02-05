#include "IndividualGameMode.h"
#include "IndividualItem.h"
#include "TestCharacter.h"

AIndividualGameMode::AIndividualGameMode()
{
	DefaultPawnClass = ATestCharacter::StaticClass();
	
	// CurrentItemCount = 0;
	// MaxCount = 10;
	// MinCount = 0;

	// IndividualItems.Init(nullptr, MaxCount);
	
	// for (auto arr : IndividualItems)
	// {
		// auto Item = GetWorld()->SpawnActor(AIndividualItem::StaticClass());
		// IndividualItems.Emplace(Item);
		// UE_LOG(LogTemp, Warning, TEXT("실행 !"));
	// }

	// GetWorld()->SpawnActor<AIndividualItem>(AIndividualItem::StaticClass());
}

void AIndividualGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}

void AIndividualGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
}

// void AIndividualGameMode::SpawnAndInsertTArray()
// {
	// auto Item = GetWorld()->SpawnActor(AIndividualItem::StaticClass());
	// IndividualItems.Insert(Item, CurrentItemCount);
// }