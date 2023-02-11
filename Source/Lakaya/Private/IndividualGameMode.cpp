#include "IndividualGameMode.h"
#include "IndividualItem.h"
#include "TestCharacter.h"

AIndividualGameMode::AIndividualGameMode()
{
	DefaultPawnClass = ATestCharacter::StaticClass();

	ItemMaxCount = 3;

	PosMinCount = 1;
	PosMaxCount = 6;
	
	// TODO : 하드코딩 수정, 위치값을 불러오는 방법 알아보자.
	pos_01 = (FVector(1000.0f, 200.0f, 0.0f));
	pos_02 = (FVector(1000.0f, 400.0f, 0.0f));
	pos_03 = (FVector(1000.0f, 600.0f, 0.0f));
	pos_04 = (FVector(1000.0f, 800.0f, 0.0f));
	pos_05 = (FVector(1000.0f, 1000.0f, 0.0f));
	pos_06 = (FVector(1000.0f, 1200.0f, 0.0f));
}

void AIndividualGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	for (int i = 0; i < ItemMaxCount; i++)
	{
		RandomSpawn();
	}
}

void AIndividualGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AIndividualGameMode::RandomSpawn()
{
	int32 PosNumber = FMath::RandRange(PosMinCount, PosMaxCount);
	AActor* item = GetWorld()->SpawnActor(AIndividualItem::StaticClass());
	
	switch (PosNumber)
	{
	case 1:
		item->SetActorRelativeLocation(FVector(pos_01));
		break;
	case 2:
		item->SetActorRelativeLocation(FVector(pos_02));
		break;
	case 3:
		item->SetActorRelativeLocation(FVector(pos_03));
		break;
	case 4:
		item->SetActorRelativeLocation(FVector(pos_04));
		break;
	case 5:
		item->SetActorRelativeLocation(FVector(pos_05));
		break;
	case 6:
		item->SetActorRelativeLocation(FVector(pos_06));
		break;
	}

	Item.Add(item);
}