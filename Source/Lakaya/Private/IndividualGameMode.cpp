#include "IndividualGameMode.h"
#include "ArmedCharacter.h"
#include "MovableCharacter.h"
#include "CollectorPlayerState.h"
#include "IndividualItem.h"
#include "MenuCallingPlayerController.h"
#include "TestCharacter.h"
#include "DamageableCharacter.h"
#include "GameFramework/PlayerStart.h"

AIndividualGameMode::AIndividualGameMode()
{
	DefaultPawnClass = AArmedCharacter::StaticClass();
	PlayerControllerClass = AMenuCallingPlayerController::StaticClass();
	PlayerStateClass = ACollectorPlayerState::StaticClass();

	NumPlayers = 0;
}

void AIndividualGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// GameState = EGameState::Wait;
	
	InitRandomSpawn();
}

void AIndividualGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	NumPlayers++;

	UE_LOG(LogTemp, Warning, TEXT("The Player has entered the game."));
	UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), NumPlayers);

	
	
	// TODO : 특정 조건에 충족하면, 게임 시작.
	// if (NumPlayers == MaxPlayers)
	// {
	//		TODO : Start the game
	//		GameState = EGameState::Progress;
	// }
}

void AIndividualGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	NumPlayers--;
	UE_LOG(LogTemp, Warning, TEXT("The Player has left the game."));
	UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), NumPlayers);

	// TODO : 게임종료 조건
	// if ()
	// {
	// 	GameState = EGameState::Finish;
	//
	// 	TODO : Cleanup and exit the game;
	// }
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

void AIndividualGameMode::OnKilledCharacter(AController* KilledCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("OnKillCharacter has been called !"));

	if (KilledCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IndividualGameMode_KilledCharacter is null."));	
		return;
	}

	TArray<AActor*> PlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartActors);
	
	if (PlayerStartActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Player start actors found."));
		return;
	}

	APlayerStart* RandomPlayerStart = Cast<APlayerStart>(PlayerStartActors[FMath::RandRange(0, PlayerStartActors.Num() -1)]);

	if (APawn* KilledPlayerPawn = Cast<APawn>(KilledCharacter))
	{
		FVector NewLocation = RandomPlayerStart->GetActorLocation();
		KilledPlayerPawn->SetActorLocation(NewLocation);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("IndividualGameMode_KilledCharacter is null."));
		return;
	}
}

// void AIndividualGameMode::OnKillCharacter(AController* EventInstigator, AActor* DamageCauser)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("OnKillCharacter has been called !"));
//
// 	if (EventInstigator == nullptr || DamageCauser == nullptr)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("One or both of the inputs is null."));
// 		return;
// 	}
//
// 	TArray<AActor*> PlayerStartActors;
// 	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartActors);
//
// 	if (PlayerStartActors.Num() == 0)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("No Player start actors found."));
// 		return;
// 	}
// 	
// 	APlayerStart* RandomPlayerStart = Cast<APlayerStart>(PlayerStartActors[FMath::RandRange(0, PlayerStartActors.Num() -1)]);
//
// 	if (APawn* DeadPlayerPawn = Cast<APawn>(DamageCauser))
// 	{
// 		FVector NewLocation = RandomPlayerStart->GetActorLocation();
// 		DeadPlayerPawn->SetActorLocation(NewLocation);
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("DeadPlayerPawn is null."));
// 		return;
// 	}
// }