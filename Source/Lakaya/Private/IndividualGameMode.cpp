#include "IndividualGameMode.h"
#include "ArmedCharacter.h"
#include "MovableCharacter.h"
#include "CollectorPlayerState.h"
#include "IndividualItem.h"
#include "MenuCallingPlayerController.h"
#include "TestCharacter.h"

AIndividualGameMode::AIndividualGameMode()
{
	DefaultPawnClass = AArmedCharacter::StaticClass();
	PlayerControllerClass = AMenuCallingPlayerController::StaticClass();
	PlayerStateClass = ACollectorPlayerState::StaticClass();
	
	// ItemMaxCount = 3;
	// PosMinCount = 1;
	// PosMaxCount = 6;
	// PosX = 1000;

	// Minutes = 3;
	// Seconds = 1;
}

void AIndividualGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	for (int i = 0; i < ItemMaxCount; i++)
	{
		InitRandomSpawn();
	}
}

void AIndividualGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AIndividualGameMode::InitRandomSpawn()
{
	int32 PosNumber = FMath::RandRange(PosMinCount, PosMaxCount);

	bool bContain = VectorArray.Contains(PosNumber);
	if (!bContain)
	{
		VectorArray.Add(PosNumber);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("중복"));
		InitRandomSpawn();
		return;
	}

	AActor* item = GetWorld()->SpawnActor(AIndividualItem::StaticClass());
	AIndividualItem* DividualItem = Cast<AIndividualItem>(item);
	DividualItem->ItemNumber = PosNumber;

	// TODO : 상호작용 오브젝트 나오면 위치 배치 후 오브젝트 위치 값을 불러오기.
	switch (PosNumber)
	{
	case 1:
		item->SetActorRelativeLocation(FVector(ItemPositions[0]));
		break;
	case 2:
		item->SetActorRelativeLocation(FVector(ItemPositions[1]));
		break;
	case 3:
		item->SetActorRelativeLocation(FVector(ItemPositions[2]));
		break;
	case 4:
		item->SetActorRelativeLocation(FVector(ItemPositions[3]));
		break;
	case 5:
		item->SetActorRelativeLocation(FVector(ItemPositions[4]));
		break;
	case 6:
		item->SetActorRelativeLocation(FVector(ItemPositions[5]));
		break;
	}
	
}

void AIndividualGameMode::SpawnItem()
{
	// TODO : 시간 조정.
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnItem, this, &AIndividualGameMode::InitRandomSpawn, 1.0f, false);
}

// int32 AIndividualGameMode::GetMinutes()
// {
// 	return Minutes;
// }
//
// int32 AIndividualGameMode::GetSeconds()
// {
// 	return Seconds;
// }
//
// int32 AIndividualGameMode::SetMinutes(int32 Value)
// {
// 	return Minutes = Value;
// }
//
// int32 AIndividualGameMode::SetSeconds(int32 Value)
// {
// 	return Seconds = Value;
// }
//
// int32 AIndividualGameMode::MinusMinutes(int32 Value)
// {
// 	return Minutes -= Value;
// }
//
// int32 AIndividualGameMode::MinusSeconds(int32 Value)
// {
// 	return Seconds -= Value;
// }