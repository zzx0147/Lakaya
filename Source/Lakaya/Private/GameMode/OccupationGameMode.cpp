// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/OccupationGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Character/ArmedCharacter.h"
#include "Character/BattlePlayerController.h"
#include "Character/CollectorPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "GameMode/OccupationGameState.h"

AOccupationGameMode::AOccupationGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnObject(TEXT("/Game/Characters/LakayaCharacter/Dummy/BP_PlayerDummy"));
	if (!PlayerPawnObject.Succeeded())
	{
		UE_LOG(LogTemp, Error, TEXT("OccupationGameMode_Failed to find player pawn blueprint."));
		return;
	}

	DefaultPawnClass = PlayerPawnObject.Class;
	PlayerControllerClass = ABattlePlayerController::StaticClass();
	PlayerStateClass = ACollectorPlayerState::StaticClass();
	GameStateClass = AOccupationGameState::StaticClass();
}

void AOccupationGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AOccupationGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnGameModeInitialized.Broadcast();
}

void AOccupationGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AOccupationGameState* OccupationGameState = GetWorld()->GetGameState<AOccupationGameState>();
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_OccupationGameState is null."));
		return;
	}

	int32 CurrentPlayerNum = OccupationGameState->PlayerArray.Num();
	OccupationGameState->SetNumPlayers(CurrentPlayerNum);
	
	if (GetNumPlayers() >= OccupationGameState->GetMaxPlayers())
	{
		GetWorldTimerManager().SetTimer(TimerHandle_DelayedStart, this, &AOccupationGameMode::DelayedStartMatch, 5.0f, false);
	}
}

void AOccupationGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	AOccupationGameState* OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode_OccupationGameState is null."));
		return;
	}
	
	OccupationGameState->SetGameState(EOccupationGameState::StandByToPregressLoading);
	
	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleMatchIsWaitingToStart"));

	// 게임시작 조건
	ReadyToStartMatch();
}

bool AOccupationGameMode::ReadyToStartMatch_Implementation()
{
	if (GetMatchState() != MatchState::WaitingToStart) return false;

	if (!bWaitToStart) return false;

	AOccupationGameState* OccupationGameState = Cast<AOccupationGameState>(GetWorld()->GetGameState());
	if (OccupationGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_OccupationGameState is null."));
		return false;
	}
	
	for (int i = 0; i < OccupationGameState->GetMaxPlayers(); i++)
	{
		if (OccupationGameState->PlayerArray.IsValidIndex(i))
		{
			ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(OccupationGameState->PlayerArray[i]);
			if (CollectorPlayerState == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_CollectorPlayerState is null."));
				return false;
			}

			if (i % 2 == 0)
			{
				CollectorPlayerState->SetPlayerTeamState(EPlayerTeamState::A);
				UE_LOG(LogTemp, Warning, TEXT("A팀에 배정 되었습니다."));
			}
			else
			{
				CollectorPlayerState->SetPlayerTeamState(EPlayerTeamState::B);
				UE_LOG(LogTemp, Warning, TEXT("B팀에 배정 되었습니다."));
			}
		}
	}

	OccupationGameState->SetGameState(EOccupationGameState::Progress);
	
	return true;
}

void AOccupationGameMode::DelayedStartMatch()
{
	Super::DelayedStartMatch();
}

void AOccupationGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void AOccupationGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}

void AOccupationGameMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();
}

void AOccupationGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void AOccupationGameMode::OnKilledCharacter(AController* VictimController, AActor* Victim,
	AController* InstigatorController, AActor* DamageCauser)
{
	Super::OnKilledCharacter(VictimController, Victim, InstigatorController, DamageCauser);
}

void AOccupationGameMode::OnKillNotifyBinding()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADamageableCharacter::StaticClass(), FoundActors);
	
	for (auto Actor : FoundActors)
	{
		ADamageableCharacter* MyActor = Cast<ADamageableCharacter>(Actor);
		if (MyActor)
		{
			MyActor->OnKillCharacterNotify.AddUObject(this, &AOccupationGameMode::OnKilledCharacter);
			UE_LOG(LogTemp, Warning, TEXT("PlayerController OnKillCharacterNotify Binding."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("MyActor is null."));
			return;
		}
	}
}

void AOccupationGameMode::RespawnPlayer(AController* KilledController)
{
	ACollectorPlayerState* CollectorPlayerState = Cast<ACollectorPlayerState>(KilledController->PlayerState);
	if (CollectorPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LakayaDefaultPlayGameMode_CollectorPlayerState is null."));
		return;
	}

	FName SpawnTag;
	switch (CollectorPlayerState->GetPlayerTeamState())
	{
	case EPlayerTeamState::A:
		SpawnTag = FName("ATeamSpawnZone");
		break;
	case EPlayerTeamState::B:
		SpawnTag = FName("BTeamSpawnZone");
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid player team state."));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("SpawnTag: %s"), *SpawnTag.ToString());
	
	TArray<AActor*> PlayerStartActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), SpawnTag, PlayerStartActors);

	UE_LOG(LogTemp, Warning, TEXT("PlayerStartActors.Num(): %d"), PlayerStartActors.Num());
	
	if (PlayerStartActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Player Start Actors found."));
		return;
	}

	APlayerStart* PlayerStart = Cast<APlayerStart>(PlayerStartActors[FMath::RandRange(0, PlayerStartActors.Num() - 1)]);
	APawn* KilledPawn = Cast<APawn>(KilledController->GetPawn());
	ACharacter* KilledCharacterActor = Cast<ACharacter>(KilledController->GetCharacter());
	
	if (KilledPawn != nullptr)
	{
		KilledPawn->SetActorLocation(PlayerStart->GetActorLocation());
	}
	else if (KilledController != nullptr)
	{
		KilledCharacterActor->SetActorLocation(PlayerStart->GetActorLocation());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_KilledCharacter is not a pawn or an actor."));
		return;
	}

	ADamageableCharacter* KilledDamageableCharacter = Cast<ADamageableCharacter>(KilledCharacterActor);
	if (KilledDamageableCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("KilledDamageableCharacter is null."));
		return;
	}
	
	KilledDamageableCharacter->FullHealth();
	KilledDamageableCharacter->Respawn();
}