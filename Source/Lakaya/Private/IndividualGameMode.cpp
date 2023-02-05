#include "IndividualGameMode.h"
#include "IndividualItem.h"
#include "TestCharacter.h"

AIndividualGameMode::AIndividualGameMode()
{
	DefaultPawnClass = ATestCharacter::StaticClass();

	IndividualItems.Init(nullptr, 10);
	
	CurrentItemCount = 0;
	MaxCount = 10;
	MinCount = 0;

	UE_LOG(LogTemp, Warning, TEXT("%d"), IndividualItems.Num());

	for(int i = 0; i < 10; i++)
	{
		// 내용물 뜯어보기.
	}
}

void AIndividualGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AIndividualGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AIndividualGameMode::SpawnAndInsertTArray()
{
	// auto Item = GetWorld()->SpawnActor(AIndividualItem::StaticClass());
	// IndividualItems.Insert(Item, CurrentItemCount);
}