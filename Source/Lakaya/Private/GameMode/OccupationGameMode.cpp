#include "GameMode/OccupationGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Character/ArmedCharacter.h"
#include "PlayerController/BattlePlayerController.h"
// #include "Character/CollectorPlayerState.h"
#include "Character/OccupationPlayerState.h"
#include "GameMode/OccupationGameState.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/OccupationPlayerController.h"

AOccupationGameMode::AOccupationGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnObject(TEXT("/Game/Characters/LakayaCharacter/Dummy/BP_PlayerDummy"));
	if (!PlayerPawnObject.Succeeded())
	{
		UE_LOG(LogTemp, Error, TEXT("OccupationGameMode_Failed to find player pawn blueprint."));
		return;
	}
	
	GameStateClass = AOccupationGameState::StaticClass();
	DefaultPawnClass = PlayerPawnObject.Class;
	PlayerControllerClass = AOccupationPlayerController::StaticClass();
	PlayerStateClass = AOccupationPlayerState::StaticClass();
}

void AOccupationGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AOccupationGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AOccupationGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	OccupationGameState = GetWorld()->GetGameState<AOccupationGameState>();
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
	
	OccupationGameState->SetGameState(EOccupationGameState::StandByToPregressLoading);
	UE_LOG(LogTemp, Log, TEXT("HandleMatchIsWaitingToStart"));
}

bool AOccupationGameMode::ReadyToStartMatch_Implementation()
{
	if (GetMatchState() != MatchState::WaitingToStart) return false;
	
	if (!GetbWaitToStart()) return false;
	
	for (int i = 0; i < OccupationGameState->GetMaxPlayers(); i++)
	{
		if (OccupationGameState->PlayerArray.IsValidIndex(i))
		{
			AOccupationPlayerState* OccupationPlayerState = Cast<AOccupationPlayerState>(OccupationGameState->PlayerArray[i]);
			if (OccupationPlayerState == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_PlayerState is null."));
				return false;
			}
			
			if (i % 2 == 0)
			{
				OccupationPlayerState->SetPlayerTeamState(EPlayerTeamState::A);
				UE_LOG(LogTemp, Warning, TEXT("A팀에 배정 되었습니다."));
			}
			else
			{
				OccupationPlayerState->SetPlayerTeamState(EPlayerTeamState::B);
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
	OccupationGameState->OnMatchStarted(GamePlayTime);
	OnKillNotifyBinding();

	// 플레이어 인원만큼 위치를 조정해줍니다. (각각의 팀 위치에서)
	PlayerInitializeSetLocation(OccupationGameState->PlayerArray.Num());
	
	UE_LOG(LogTemp, Error, TEXT("HandleMatchHasStarted"));
}

void AOccupationGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	OccupationGameState->SetGameState(EOccupationGameState::Finish);
	
	if (OccupationGameState->GetATeamScore() > OccupationGameState->GetBTeamScore())
	{
		OccupationGameState->SetOccupationWinner(EOccupationWinner::A);
	}
	else
	{
		OccupationGameState->SetOccupationWinner(EOccupationWinner::B);
	}
	
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedEnded, this, &AOccupationGameMode::DelayedEndedGame, 2.0f, false);

}

void AOccupationGameMode::DelayedEndedGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainLobbyLevel");
}

void AOccupationGameMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();
}

void AOccupationGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

// void AOccupationGameMode::OnKilledCharacter(AController* VictimController, AActor* Victim,
// 	AController* InstigatorController, AActor* DamageCauser)
// {
// 	Super::OnKilledCharacter(VictimController, Victim, InstigatorController, DamageCauser);
// }

void AOccupationGameMode::OnKillNotifyBinding()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADamageableCharacter::StaticClass(), FoundActors);
	
	for (auto& Actor : FoundActors)
	{
		ADamageableCharacter* MyActor = Cast<ADamageableCharacter>(Actor);
		if (MyActor)
		{
			// MyActor->OnKillCharacterNotify.AddUObject(this, &AOccupationGameMode::OnKilledCharacter);
			// MyActor->OnAliveChanged.AddUObject(this, &AOccupationGameMode::OnKilledCharacter);
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
	AOccupationPlayerState* OccupationPlayerState = Cast<AOccupationPlayerState>(KilledController->PlayerState);
	if (OccupationPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_OccupationPlayerState is null."));
		return;
	}

	FName SpawnTag;
	switch (OccupationPlayerState->GetPlayerTeamState())
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
	
	KilledDamageableCharacter->Respawn();
}

void AOccupationGameMode::PlayerInitializeSetLocation(uint8 PlayersNum)
{
	for (int i = 0; PlayersNum; i++)
	{
		if (OccupationGameState->PlayerArray.IsValidIndex(i))
		{
			AOccupationPlayerState* OccupationPlayerState = Cast<AOccupationPlayerState>(OccupationGameState->PlayerArray[i]);
			if (OccupationPlayerState == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("LakayaDefaultPlayGameMode_OccupationPlayerState is null."));
				return;
			}
			
			AController* OccuController = Cast<AController>(OccupationPlayerState->GetOwner());
			if (OccuController == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_PlayerController is null."));
				return;
			}

			FName SpawnTag;
			switch (OccupationPlayerState->GetPlayerTeamState())
			{
			case EPlayerTeamState::A:
				SpawnTag = FName("ATeamSpawnZone");
				break;
			case EPlayerTeamState::B:
				SpawnTag = FName("BTeamSpawnZone");
				break;
			default:
				UE_LOG(LogTemp, Warning, TEXT("Invalid player steam state."))	
				break;
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
			APawn* ArgCharacterPawn = Cast<APawn>(OccuController->GetPawn());
			ACharacter* ArgCharacterActor = Cast<ACharacter>(OccuController->GetCharacter());

			if (ArgCharacterPawn != nullptr)
			{
				ArgCharacterPawn->SetActorLocation(PlayerStart->GetActorLocation());
			}
			else if (ArgCharacterActor != nullptr)
			{
				ArgCharacterActor->SetActorLocation(PlayerStart->GetActorLocation());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_KilledCharacter is not a pawn or an actor."));
			}

			UE_LOG(LogTemp, Warning, TEXT("Player SetLocation Completed."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerArray is Not Valid."));
			return;
		}
	}
}