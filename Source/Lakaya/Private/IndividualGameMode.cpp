#include "IndividualGameMode.h"
#include "ArmedCharacter.h"
#include "MovableCharacter.h"
#include "CollectorPlayerState.h"
#include "IndividualItem.h"
#include "MenuCallingPlayerController.h"
#include "TestCharacter.h"
#include "DamageableCharacter.h"

AIndividualGameMode::AIndividualGameMode()
{
	DefaultPawnClass = AArmedCharacter::StaticClass();
	PlayerControllerClass = AMenuCallingPlayerController::StaticClass();
	PlayerStateClass = ACollectorPlayerState::StaticClass();
}

void AIndividualGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitRandomSpawn();
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

	ItemNumCheck();
}

void AIndividualGameMode::SpawnItem()
{
	// TODO : 기획서에 맞게 시간 수정.
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnItem, this, &AIndividualGameMode::InitRandomSpawn, 1.0f, false);
}

void AIndividualGameMode::ItemNumCheck()
{
	int32 NumSpawnedItems = VectorArray.Num();

	if (NumSpawnedItems < ItemMaxCount)
		GetWorldTimerManager().SetTimer(TimerHandle_SpawnItem, this, &AIndividualGameMode::InitRandomSpawn, 1.0f, false);
}