#include "IndividualGameMode.h"
#include "ArmedCharacter.h"
#include "MovableCharacter.h"
#include "CollectorPlayerState.h"
#include "IndividualEnergy.h"
#include "MenuCallingPlayerController.h"
#include "DamageableCharacter.h"
#include "IndividualStaticEnergy.h"
#include "InteractableCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "IndividualDropEnergy.h"
#include "DropEnergyPool.h"
#include "EngineUtils.h"

AIndividualGameMode::AIndividualGameMode()
{
	DefaultPawnClass = AInteractableCharacter::StaticClass();
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

	SpawnStaticEnergyAtRandomPosition();
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

void AIndividualGameMode::SpawnStaticEnergyAtRandomPosition()
{
	int32 PosNumber = FMath::RandRange(PosMinCount, PosMaxCount);

	bool bContain = VectorArray.Contains(PosNumber);
	if (!bContain)
	{
		VectorArray.Add(PosNumber);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Duplication."));
		SpawnStaticEnergyAtRandomPosition();
		return;
	}

	AActor* item = GetWorld()->SpawnActor(AIndividualStaticEnergy::StaticClass());
	AIndividualStaticEnergy* DividualItem = Cast<AIndividualStaticEnergy>(item);
	DividualItem->StaticEnergyNumber = PosNumber;

	// TODO : 상호작용 오브젝트 나오면 위치 배치 후 오브젝트 위치 값을 불러오기.
	switch (PosNumber)
	{
	case 1:
		item->SetActorRelativeLocation(FVector(StaticEnergyPositions[0]));
		break;
	case 2:
		item->SetActorRelativeLocation(FVector(StaticEnergyPositions[1]));
		break;
	case 3:
		item->SetActorRelativeLocation(FVector(StaticEnergyPositions[2]));
		break;
	case 4:
		item->SetActorRelativeLocation(FVector(StaticEnergyPositions[3]));
		break;
	case 5:
		item->SetActorRelativeLocation(FVector(StaticEnergyPositions[4]));
		break;
	case 6:
		item->SetActorRelativeLocation(FVector(StaticEnergyPositions[5]));
		break;
	}

	StaticEnergyNumCheck();
}

void AIndividualGameMode::SpawnStaticEnergy()
{
	// TODO : 기획서에 맞게 시간 수정.
	GetWorldTimerManager().SetTimer(TimerHandle_SpawnStaticEnergy, this, &AIndividualGameMode::SpawnStaticEnergyAtRandomPosition, 1.0f, false);
}

void AIndividualGameMode::SpawnDropEnergy(AController* DeadPlayer)
{
	// TODO : 오브젝트풀링을 이용해서 기존에 생성되었던 드랍에너지를 가져와서
	// 사망한 플레이어에게서 드랍된 것처럼 보이게끔 구현.
	AIndividualDropEnergy* DropEnergy = DropEnergyPool->GetDropEnergy();
	if (DropEnergy == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("DropEnergy is null."));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Pool Num : %d"), DropEnergyPool->InactiveDropEnergys.Num());

	// 위치 조정
	DropEnergy->SetDropEnergy(DeadPlayer);

	// 생명 주기
}

void AIndividualGameMode::StaticEnergyNumCheck()
{
	int32 SpawnedStaticEnergyNum = VectorArray.Num();

	if (SpawnedStaticEnergyNum < StaticEnergyMaxCount)
		GetWorldTimerManager().SetTimer(TimerHandle_SpawnStaticEnergy, this, &AIndividualGameMode::SpawnStaticEnergyAtRandomPosition, 1.0f, false);
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

	// Spawn Drop Energy
	for (uint8 i = 0 ; i < CollectorPlayerState->GetEnergy(); i++)
	{
		SpawnDropEnergy(VictimController);
		UE_LOG(LogTemp, Warning, TEXT("For SpawnDropEnergy."));
	}
	
	if (VictimController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IndividualGameMode_KilledCharacter is null."));
		return;
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