#include "IndividualGameMode.h"
#include "ArmedCharacter.h"
#include "MovableCharacter.h"
#include "CollectorPlayerState.h"
#include "IndividualItem.h"
#include "MenuCallingPlayerController.h"
#include "TestCharacter.h"
#include "DamageableCharacter.h"
#include "InteractableCharacter.h"
#include "GameFramework/PlayerStart.h"

AIndividualGameMode::AIndividualGameMode()
{
	// DefaultPawnClass = AArmedCharacter::StaticClass();
	DefaultPawnClass = AInteractableCharacter::StaticClass();
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

void AIndividualGameMode::OnKilledCharacter(AController* KilledCharacter, AController* EventInstigator)
{
	UE_LOG(LogTemp, Warning, TEXT("OnKillCharacter has been called !"));

	if (EventInstigator == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IndividualGameMode_EventInstigator is null."));
		return;
	}

	// Get the player state from the event instigator's controller
	ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(EventInstigator->GetCharacter()->GetController()->PlayerState);
	if (CollectorPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("CollectorPlayerState is null."));
		return;
	}

	// Award the player 2 points for the kill
	CollectorPlayerState->GainPoint(2);
	CollectorPlayerState->GainMoney(1);
	UE_LOG(LogTemp, Warning, TEXT("Player %s has gained 2 points."), *CollectorPlayerState->GetPlayerName());
	UE_LOG(LogTemp, Warning, TEXT("Player Total points: %d"), CollectorPlayerState->GetPoint());
	UE_LOG(LogTemp, Warning, TEXT("Player Totla Money : %d"), CollectorPlayerState->GetMoney());
	
	if (KilledCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IndividualGameMode_KilledCharacter is null."));
		return;
	}

	// Find a random player start location
	TArray<AActor*> PlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartActors);

	if (PlayerStartActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No player start actors found."));
		return;
	}

	APlayerStart* RandomPlayerStart = Cast<APlayerStart>(PlayerStartActors[FMath::RandRange(0, PlayerStartActors.Num() - 1)]);

	APawn* KilledPawn = Cast<APawn>(KilledCharacter->GetPawn());
	ACharacter* KilledCharacterActor = Cast<ACharacter>(KilledCharacter->GetCharacter());
	
	if (KilledPawn != nullptr)
	{
		KilledPawn->SetActorLocation(RandomPlayerStart->GetActorLocation());
	}
	else if (KilledCharacter != nullptr)
	{
		KilledCharacterActor->SetActorLocation(RandomPlayerStart->GetActorLocation());
	}
	else
	{
			UE_LOG(LogTemp, Warning, TEXT("KilledCharacter is not a pawn or an actor."));
			return;
	}

	ACollectorPlayerState* KilledPlayerState = Cast<ACollectorPlayerState>(KilledCharacter->GetCharacter()->GetController()->PlayerState);
	if (KilledCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("KilledPlayerState is null."));
		return;
	}

	ADamageableCharacter* KilledDamageableCharacter = Cast<ADamageableCharacter>(KilledCharacterActor);
	if (KilledDamageableCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("KilledDamageableCharacter is null."));
		return;
	}
	
	KilledPlayerState->ResetEnergy();
	KilledDamageableCharacter->FullHealth();
}