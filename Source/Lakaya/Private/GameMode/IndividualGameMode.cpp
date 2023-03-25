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

	DropEnergyPool = GetWorld()->SpawnActor<ADropEnergyPool>(ADropEnergyPool::StaticClass());
	DropEnergyPool->Initialize(30);
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
	// 사망한 플레이어에게서 드랍된 것처럼 보이게끔 구현.
	AIndividualDropEnergy* DropEnergy = DropEnergyPool->GetDropEnergy();
	if (DropEnergy == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("DropEnergy is null."));
		return;
	}
	
	// 위치 조정
	DropEnergy->SetDropEnergy(DeadPlayer);

	// TODO : N초 후에도 이 액터가 살아있다면 자동 소멸
	// 생명 주기
	// FTimerHandle TimerHandle;
	// GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, DropEnergy, &TimerHandle](const FTimerHandle InTimerHandle)
	// {
	// 	if (IsValid(DropEnergy))
	// 	{
	// 		// If time has elapsed, if DropEnergy exists (if no one has eaten it), return automatically.
	// 		DropEnergyPool->ReturnDropEnergy(DropEnergy);
	// 	}
	// 	else
	// 	{
	// 		// If DropEnergy is deleted by player interaction
	// 		// UE_LOG(LogTemp, Warning, TEXT("DropEnergy is Dropped."));
	// 		DropEnergyPool->ReturnDropEnergy(DropEnergy);
	// 	}
	//
	// 	GetWorld()->GetTimerManager().ClearTimer(InTimerHandle);
	// 	DropEnergyPool->ReturnDropEnergy(DropEnergy);
	//
	// }, 3.0f, false);

	// GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, DropEnergy, &TimerHandle](const FTimerHandle& InTimerHandle)
	// {
	// 	if(IsValid(DropEnergy)())
	// 	{
	// 		// 시간이 지났을 경우, DropEnergy가 존재하다면(아무도 먹지 않았을 경우) 자동으로 Return.
	// 		DropEnergyPool->ReturnDropEnergy(DropEnergy);
	// 	}
	// 	else
	// 	{
	// 		// DropEnergy가 플레이어의 상호작용에 의해 삭제됐을 경우
	// 		// UE_LOG(LogTemp, Warning, TEXT("DropEnergy is Dropped."));
	// 		DropEnergyPool->ReturnDropEnergy(DropEnergy);
	// 	}
	//
	// 	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	// 	DropEnergyPool->ReturnDropEnergy(DropEnergy);
	// }, 3.0f, false);
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
	UE_LOG(LogTemp, Warning, TEXT("OnKillCharacter has been called !"));
	
	if (InstigatorController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IndividualGameMode_EventInstigator is null."));
		return;
	}

	ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(InstigatorController->GetCharacter()->GetController()->PlayerState);
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
	UE_LOG(LogTemp, Warning, TEXT("Player Total Money : %d"), CollectorPlayerState->GetMoney());
	
	if (VictimController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IndividualGameMode_KilledCharacter is null."));
		return;
	}

	// Spawn Drop Energy
	for (uint8 i = 0 ; i < CollectorPlayerState->GetEnergy(); i++)
	{
		SpawnDropEnergy(VictimController);
		UE_LOG(LogTemp, Warning, TEXT("For SpawnDropEnergy."));
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