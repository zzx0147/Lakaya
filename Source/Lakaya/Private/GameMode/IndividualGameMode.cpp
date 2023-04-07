#include "GameMode/IndividualGameMode.h"
#include "Character/CollectorPlayerState.h"
#include "Character/MenuCallingPlayerController.h"
#include "Individual/IndividualStaticEnergy.h"
#include "Character/InteractableCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Individual/IndividualDropEnergy.h"
#include "EngineUtils.h"
#include "Blueprint/WidgetTree.h"
#include "Character/BattlePlayerController.h"
#include "GameMode/IndividualGameState.h"
#include "Net/UnrealNetwork.h"
#include "UI/LoadingWidget.h"

AIndividualGameMode::AIndividualGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnObject(TEXT("/Game/Characters/LakayaCharacter/Dummy/BP_PlayerDummy"));
	if (!PlayerPawnObject.Succeeded())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find player pawn blueprint."));
		return;
	}

	static ConstructorHelpers::FClassFinder<AIndividualGameState> GameStateFinder(TEXT("/Game/Blueprints/GameModes/MyIndividualGameState.MyIndividualGameState_C"));
	if (!GameStateFinder.Succeeded())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find gamestate blueprint."));
		return;
	}
	
	DefaultPawnClass = PlayerPawnObject.Class;
	PlayerControllerClass = ABattlePlayerController::StaticClass();
	PlayerStateClass = ACollectorPlayerState::StaticClass();
	GameStateClass = GameStateFinder.Class;
}

void AIndividualGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AIndividualGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnGameModeInitialized.Broadcast();
}

void AIndividualGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	// UE_LOG(LogTemp, Warning, TEXT("The Player has entered the game."));
	// UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), GetNumPlayers());

	// GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("플레이어가 입장했습니다."));
	
	AIndividualGameState* IndividualGameState = GetWorld()->GetGameState<AIndividualGameState>();
	if (IndividualGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IndividualGameMode_IndividualGameState is null."));
		return;
	}

	int32 CurrentPlayerNum = IndividualGameState->PlayerArray.Num();
	IndividualGameState->SetNumPlayers(CurrentPlayerNum);

	if (GetNumPlayers() >= IndividualGameState->GetMaxPlayers())
	{
		GetWorldTimerManager().SetTimer(TimerHandle_DelayedStart, this, &AIndividualGameMode::DelayedStartMatch, 5.0f, false);
	}
}

void AIndividualGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	AIndividualGameState* IndividualGameState = Cast<AIndividualGameState>(GetWorld()->GetGameState());
	if (IndividualGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode_IndividualGameState is null."));
		return;
	}
	
	IndividualGameState->SetGameState(EIndividualGameState::StandByToPregressLoading);
	
	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleMatchIsWaitingToStart"));

	// 게임시작 조건
	ReadyToStartMatch();
}

bool AIndividualGameMode::ReadyToStartMatch_Implementation()
{
	if (GetMatchState() != MatchState::WaitingToStart)
	{
		return false;
	}

	if (!bWaitToStart) return false;

	AIndividualGameState* IndividualGameState = Cast<AIndividualGameState>(GetWorld()->GetGameState());
	if (IndividualGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode_IndividualGameSate is null."));
		return false;
	}
	
	IndividualGameState->SetGameState(EIndividualGameState::Progress);
	
	return true;
}

void AIndividualGameMode::DelayedStartMatch()
{
	Super::DelayedStartMatch();
	
	// bWaitToStart = true;
}

void AIndividualGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	OnKillNotifyBinding();
	
	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleMatchHasStarted"));
}

void AIndividualGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}

void AIndividualGameMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();
}

void AIndividualGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	// ADamageableCharacter* DamageableCharacter = Cast<ADamageableCharacter>(Exiting->GetPawn());
	// if (DamageableCharacter)
	// {
	// 	DamageableCharacter->OnKillCharacterNotify.RemoveAll(this);	
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("GameMode_DamageableCharacter is null."));
	// 	return;
	// }
	//
	// UE_LOG(LogTemp, Warning, TEXT("The Player has left the game."));
	// UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), NumPlayers);
}

void AIndividualGameMode::OnKillNotifyBinding()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADamageableCharacter::StaticClass(), FoundActors);
	
	for (auto Actor : FoundActors)
	{
		ADamageableCharacter* MyActor = Cast<ADamageableCharacter>(Actor);
		if (MyActor)
		{
			MyActor->OnKillCharacterNotify.AddUObject(this, &AIndividualGameMode::OnKilledCharacter);
			UE_LOG(LogTemp, Warning, TEXT("PlayerController OnKillCharacterNotify Binding."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MyActor is null."));
			return;
		}
	}
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
	Super::RespawnPlayer(KilledController);
	// TArray<AActor*> PlayerStartActors;
	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartActors);
	//
	// if (PlayerStartActors.Num() == 0)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("No player start actors found."));
	// 	return;
	// }
	//
	// APlayerStart* RandomPlayerStart = Cast<APlayerStart>(PlayerStartActors[FMath::RandRange(0, PlayerStartActors.Num() - 1)]);
	//
	// APawn* KilledPawn = Cast<APawn>(KilledController->GetPawn());
	// ACharacter* KilledCharacterActor = Cast<ACharacter>(KilledController->GetCharacter());
	//
	// if (KilledPawn != nullptr)
	// {
	// 	KilledPawn->SetActorLocation(RandomPlayerStart->GetActorLocation());
	// }
	// else if (KilledController != nullptr)
	// {
	// 	KilledCharacterActor->SetActorLocation(RandomPlayerStart->GetActorLocation());
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("KilledCharacter is not a pawn or an actor."));
	// 	return;
	// }
	//
	ACollectorPlayerState* KilledPlayerState = Cast<ACollectorPlayerState>(KilledController->GetCharacter()->GetController()->PlayerState);
	if (KilledController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("KilledPlayerState is null."));
		return;
	}
	//
	// ADamageableCharacter* KilledDamageableCharacter = Cast<ADamageableCharacter>(KilledCharacterActor);
	// if (KilledDamageableCharacter == nullptr)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("KilledDamageableCharacter is null."));
	// 	return;
	// }
	//
	KilledPlayerState->ResetEnergy();
	// KilledDamageableCharacter->FullHealth();
	// KilledDamageableCharacter->Respawn();
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
	for (uint8 i = 0 ; i <= VictimCollectorPlayerState->GetEnergy(); i++)
	{
		SpawnDropEnergy(VictimController);
	}

	Super::OnKilledCharacter(VictimController, Victim, InstigatorController, DamageCauser);
	// FTimerHandle* ExistingTimer = RespawnTimers.Find(VictimController);
	// if (ExistingTimer != nullptr)
	// {
	// 	GetWorldTimerManager().ClearTimer(*ExistingTimer);
	// 	RespawnTimers.Remove(VictimController);
	// }
	//
	// TArray<AController*> DeadPlayers;
	// DeadPlayers.Add(VictimController);
	//
	// for (auto& Pair : RespawnTimers)
	// {
	// 	FTimerHandle& Timer = Pair.Value;
	// 	AController* Player = Pair.Key;
	//
	// 	if (GetWorldTimerManager().IsTimerActive(Timer))
	// 	{
	// 		DeadPlayers.Add(Player);
	// 	}
	// }
	//
	// for (AController* DeadPlayer : DeadPlayers)
	// {
	// 	FTimerHandle NewTimer;
	// 	GetWorldTimerManager().SetTimer(NewTimer, [this, DeadPlayer](){ RespawnPlayer(DeadPlayer); }, PlayerRespawnTime, false);
	// 	RespawnTimers.Add(DeadPlayer, NewTimer);
	// }
}

#pragma region Object Pool
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
#pragma endregion 