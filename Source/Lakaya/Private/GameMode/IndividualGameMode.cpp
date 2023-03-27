#include "GameMode/IndividualGameMode.h"
#include "Character/CollectorPlayerState.h"
#include "Character/MenuCallingPlayerController.h"
#include "Individual/IndividualStaticEnergy.h"
#include "Character/InteractableCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Individual/IndividualDropEnergy.h"
#include "Individual/DropEnergyPool.h"
#include "EngineUtils.h"

AIndividualGameMode::AIndividualGameMode()
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("/Game/Characters/LakayaCharacter/Dummy/BP_PlayerDummy"));
	if (!PlayerPawnObject.Succeeded())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find player pawn blueprint."));
		return;
	}

	UClass* PlayerPawnClass = PlayerPawnObject.Object->GeneratedClass;
	if (!PlayerPawnClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get generated class from player pawn blueprint."));
		return;
	}

	DefaultPawnClass = PlayerPawnClass;
	PlayerControllerClass = AMenuCallingPlayerController::StaticClass();
	PlayerStateClass = ACollectorPlayerState::StaticClass();
	
	NumPlayers = 0;
}

void AIndividualGameMode::BeginPlay()
{
	Super::BeginPlay();

	// DropEnergyPool = GetWorld()->SpawnActor<ADropEnergyPool>(ADropEnergyPool::StaticClass());
	// DropEnergyPool->Initialize(30);
}

void AIndividualGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AIndividualGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	OnPlayerJoined(NewPlayer);
	
	NumPlayers++;

	UE_LOG(LogTemp, Warning, TEXT("The Player has entered the game."));
	UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), NumPlayers);
}

void AIndividualGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ADamageableCharacter* DamageableCharacter = Cast<ADamageableCharacter>(Exiting->GetPawn());
	if (DamageableCharacter)
	{
		DamageableCharacter->OnKillCharacterNotify.RemoveAll(this);	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DamageableCharacter is null."));
		return;
	}
	
	NumPlayers--;
	UE_LOG(LogTemp, Warning, TEXT("The Player has left the game."));
	UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), NumPlayers);
}

void AIndividualGameMode::OnPlayerJoined(APlayerController* PlayerController)
{
	if (RegisteredPlayers.Contains(PlayerController))
	{
		return;
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADamageableCharacter::StaticClass(), FoundActors);

	for (auto Actor : FoundActors)
	{
		ADamageableCharacter* MyActor = Cast<ADamageableCharacter>(Actor);
		if (MyActor)
		{
			MyActor->OnKillCharacterNotify.AddUObject(this, &AIndividualGameMode::OnKilledCharacter);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MyActor is null."));
			return;
		}
	}

	RegisteredPlayers.Add(PlayerController);
}

void AIndividualGameMode::SpawnDropEnergy(AController* DeadPlayer)
{
	
	
	AIndividualDropEnergy* DropEnergy = GetWorld()->SpawnActor<AIndividualDropEnergy>();
	if (DropEnergy == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("DropEnergy is null."));
		return;
	}
	
	// 위치 조정
	DropEnergy->LocationSetDropEnergy(DeadPlayer);
	
	// 생명 주기
	GetWorld()->GetTimerManager().SetTimer(DropEnergy->VisibilityTimerHandle, [DropEnergy]()
   {
		DropEnergy->Destroy();
   }, 3.0f, false);
}

void AIndividualGameMode::RespawnPlayer(AController* KilledController)
{
	TArray<AActor*> PlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartActors);

	if (PlayerStartActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No player start actors found."));
		return;
	}

	APlayerStart* RandomPlayerStart = Cast<APlayerStart>(PlayerStartActors[FMath::RandRange(0, PlayerStartActors.Num() - 1)]);

	APawn* KilledPawn = Cast<APawn>(KilledController->GetPawn());
	ACharacter* KilledCharacterActor = Cast<ACharacter>(KilledController->GetCharacter());
	
	if (KilledPawn != nullptr)
	{
		KilledPawn->SetActorLocation(RandomPlayerStart->GetActorLocation());
	}
	else if (KilledController != nullptr)
	{
		KilledCharacterActor->SetActorLocation(RandomPlayerStart->GetActorLocation());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("KilledCharacter is not a pawn or an actor."));
		return;
	}

	ACollectorPlayerState* KilledPlayerState = Cast<ACollectorPlayerState>(KilledController->GetCharacter()->GetController()->PlayerState);
	if (KilledController == nullptr)
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
	KilledDamageableCharacter->Respawn();
}

void AIndividualGameMode::OnKilledCharacter(AController* VictimController, AActor* Victim, AController* InstigatorController, AActor* DamageCauser)
{
	if (InstigatorController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IndividualGameMode_EventInstigator is null."));
		return;
	}

	ACollectorPlayerState* InstigatorCollectorPlayerState = Cast<ACollectorPlayerState>(InstigatorController->GetCharacter()->GetController()->PlayerState);
	if (InstigatorCollectorPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("CollectorPlayerState is null."));
		return;
	}

	// Award the player 2 points for the kill
	InstigatorCollectorPlayerState->GainPoint(2);
	InstigatorCollectorPlayerState->GainMoney(1);
	UE_LOG(LogTemp, Warning, TEXT("Player %s has gained 2 points."), *InstigatorCollectorPlayerState->GetPlayerName());
	UE_LOG(LogTemp, Warning, TEXT("Player Total points: %d"), InstigatorCollectorPlayerState->GetPoint());
	UE_LOG(LogTemp, Warning, TEXT("Player Total Money : %d"), InstigatorCollectorPlayerState->GetMoney());
	
	if (VictimController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IndividualGameMode_KilledCharacter is null."));
		return;
	}

	ACollectorPlayerState* VictimCollectorPlayerState = Cast<ACollectorPlayerState>(VictimController->GetCharacter()->GetController()->PlayerState);
	if (VictimCollectorPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("VictimCollectorPlayerState cast failed."));
		return;
	}
	
	// Spawn Drop Energy
	for (uint8 i = 0 ; i < VictimCollectorPlayerState->GetEnergy(); i++)
	{
		SpawnDropEnergy(VictimController);
	}
	
	FTimerHandle* ExistingTimer = RespawnTimers.Find(VictimController);
	if (ExistingTimer != nullptr)
	{
		GetWorldTimerManager().ClearTimer(*ExistingTimer);
		RespawnTimers.Remove(VictimController);
	}
	
	TArray<AController*> DeadPlayers;
	DeadPlayers.Add(VictimController);
	
	for (auto& Pair : RespawnTimers)
	{
		FTimerHandle& Timer = Pair.Value;
		AController* Player = Pair.Key;
	
		if (GetWorldTimerManager().IsTimerActive(Timer))
		{
			DeadPlayers.Add(Player);
		}
	}
	
	for (AController* DeadPlayer : DeadPlayers)
	{
		FTimerHandle NewTimer;
		GetWorldTimerManager().SetTimer(NewTimer, [this, DeadPlayer](){ RespawnPlayer(DeadPlayer); }, PlayerRespawnTime, false);
		RespawnTimers.Add(DeadPlayer, NewTimer);
	}
}

// 사망한 플레이어에게서 드랍된 것처럼 보이게끔 구현.
// AIndividualDropEnergy* DropEnergy = DropEnergyPool->GetDropEnergy();
// if (DropEnergy == nullptr)
// {
// UE_LOG(LogTemp, Warning, TEXT("DropEnergy is null."));
// return;
// }

// bool AIndividualGameMode::CheckDropEnergyActorPresence(AIndividualDropEnergy* DropEnergy) const
// {
// 	// Check if the pointer to the DropEnergy actor is valid
// 	if (!DropEnergy)
// 	{
// 		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("DropEnergy actor pointer is invalid."));
// 		return false;
// 	}
// 	
// 	// Retrieve the DropEnergyActor from the game mode
// 	AIndividualDropEnergy* DropEnergyActor = DropEnergy;
//     
// 	// Check if the actor is initialized, tick enabled, and not being destroyed
// 	if (DropEnergyActor && DropEnergyActor->IsActorInitialized() && DropEnergyActor->IsActorTickEnabled() && !DropEnergyActor->IsActorBeingDestroyed())
// 	{
// 		// The client is aware of the existence of the DropEnergy actor
// 		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("DroipEnergyActor is Available."));
// 		return true;
// 	}
// 	else
// 	{
// 		// The client is not aware of the existence of the DropEnergy actor
// 		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("DroipEnergyActor is Not Available."));
// 		return false;
// 	}
// }