#include "IndividualGameMode.h"
#include "IndividualItem.h"
#include "TestCharacter.h"

AIndividualGameMode::AIndividualGameMode()
{
	DefaultPawnClass = ATestCharacter::StaticClass();

<<<<<<< HEAD
=======
	IndividualItems.Init(nullptr, 10);
	
	CurrentItemCount = 0;
	MaxCount = 10;
	MinCount = 0;

	UE_LOG(LogTemp, Warning, TEXT("%d"), IndividualItems.Num());

	for(int i = 0; i < 10; i++)
	{
		// 내용물 뜯어보기.
	}
>>>>>>> parent of 06ac0ca (20230206_0056)
}

void AIndividualGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AIndividualGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
<<<<<<< HEAD

	for (int i = 0; i < 6; i++)
	{
		AActor* item = GetWorld()->SpawnActor(AIndividualItem::StaticClass());
		ItemArray.Add(item);
		// Trigger->SetRelativeLocation(FVector(400.0f, 0.0f, 0.0f));
		item->SetActorRelativeLocation(FVector(400.0f, 200.0f * i, 0.0f));
	}

	int32 Count = ItemArray.Num();
	UE_LOG(LogTemp, Warning, TEXT("%d"), Count);
=======
}

void AIndividualGameMode::SpawnAndInsertTArray()
{
	// auto Item = GetWorld()->SpawnActor(AIndividualItem::StaticClass());
	// IndividualItems.Insert(Item, CurrentItemCount);
>>>>>>> parent of 06ac0ca (20230206_0056)
}