// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/LakayaDefaultPlayGameMode.h"

#include "Character/ArmedCharacter.h"
#include "Character/InteractableCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/LakayaBaseGameState.h"
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"

namespace MatchState
{
	 const FName IsSelectCharacter = FName(TEXT("IsSelectCharacter"));
}

const FString ALakayaDefaultPlayGameMode::ATeamSpawnTag = FString(TEXT("ATeamSpawnZone"));
const FString ALakayaDefaultPlayGameMode::BTeamSpawnTag = FString(TEXT("BTeamSpawnZone"));

ALakayaDefaultPlayGameMode::ALakayaDefaultPlayGameMode()
{
	//PlayerRespawnTime = 3;
	//bWaitToStart = false;
	bDelayedStart = true;
	MinRespawnDelay = 5.0f;
	CharacterSelectStartDelay = 3.0f;
	//CharacterSelectTime = 10.0f;
}

void ALakayaDefaultPlayGameMode::RestartPlayer(AController* NewPlayer)
{
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	FString SpawnTag;
	if (const auto BasePlayerState = NewPlayer->GetPlayerState<ALakayaBasePlayerState>())
	{
		switch (BasePlayerState->GetTeam())
		{
		case EPlayerTeam::A:
			SpawnTag = ATeamSpawnTag;
			break;
		case EPlayerTeam::B:
			SpawnTag = BTeamSpawnTag;
			break;
		default:
			SpawnTag = TEXT("");
			break;
		}
	}

	AActor* StartSpot = FindPlayerStart(NewPlayer, SpawnTag);

	// If a start spot wasn't found,
	if (StartSpot == nullptr)
	{
		// Check for a previously assigned spot
		if (NewPlayer->StartSpot != nullptr)
		{
			StartSpot = NewPlayer->StartSpot.Get();
			UE_LOG(LogGameMode, Warning, TEXT("RestartPlayer: Player start not found, using last start spot"));
		}
	}

	RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
}

void ALakayaDefaultPlayGameMode::InitStartSpot_Implementation(AActor* StartSpot, AController* NewPlayer)
{
	if (StartSpot == nullptr) return;

	if (const auto Pawn = NewPlayer->GetPawn())
	{
		Pawn->SetActorLocation(StartSpot->GetActorLocation(), false, nullptr, ETeleportType::ResetPhysics);
	}
}

AActor* ALakayaDefaultPlayGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	UWorld* World = GetWorld();

	// If incoming start is specified, then just use it
	if (!IncomingName.IsEmpty())
	{
		const FName IncomingPlayerStartTag = FName(*IncomingName);
		for (TActorIterator<APlayerStart> It(World); It; ++It)
		{
			APlayerStart* Start = *It;
			if (Start && Start->PlayerStartTag == IncomingPlayerStartTag)
			{
				if (const auto Capsule = Start->GetCapsuleComponent())
				{
					TSet<AActor*> OverlappingActors;
					Capsule->GetOverlappingActors(OverlappingActors,AInteractableCharacter::StaticClass());
					if (OverlappingActors.Num() == 0)
						return Start;
				}
			}
		}
	}

	// Always pick StartSpot at start of match
	if (ShouldSpawnAtStartSpot(Player))
	{
		if (AActor* PlayerStartSpot = Player->StartSpot.Get())
		{
			return PlayerStartSpot;
		}
		else
		{
			UE_LOG(LogGameMode, Error, TEXT("FindPlayerStart: ShouldSpawnAtStartSpot returned true but the Player StartSpot was null."));
		}
	}

	AActor* BestStart = ChoosePlayerStart(Player);
	if (BestStart == nullptr)
	{
		// No player start found
		UE_LOG(LogGameMode, Log, TEXT("FindPlayerStart: PATHS NOT DEFINED or NO PLAYERSTART with positive rating"));

		// This is a bit odd, but there was a complex chunk of code that in the end always resulted in this, so we may as well just 
		// short cut it down to this.  Basically we are saying spawn at 0,0,0 if we didn't find a proper player start
		BestStart = World->GetWorldSettings();
	}

	return BestStart;
}

void ALakayaDefaultPlayGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ALakayaDefaultPlayGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ALakayaDefaultPlayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	UE_LOG(LogTemp, Warning, TEXT("The Player has entered the game."));
	UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), GetNumPlayers());

	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("플레이어가 입장했습니다."));

	if (const auto BasePlayerState = NewPlayer->GetPlayerState<ALakayaBasePlayerState>())
	{
		BasePlayerState->OnCharacterNameChanged.AddLambda(
			[this, NewPlayer](ALakayaBasePlayerState* ArgBasePlayerState, const FName& ArgCharacterName){

				if (GetGameState<ALakayaBaseGameState>()->GetMatchState() == MatchState::InProgress)
				{
					if (auto PlayerPawn = NewPlayer->GetPawn())
					{
						NewPlayer->UnPossess();
						PlayerPawn->Destroy();
						RestartPlayer(NewPlayer);
					}
				}
			});

		BasePlayerState->OnPlayerKilled.AddUObject(this, &ALakayaDefaultPlayGameMode::OnPlayerKilled);
	}

	const auto BaseGameState = GetWorld()->GetGameState<ALakayaBaseGameState>();
	if (BaseGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_OccupationGameState is null."));
		return;
	}

	const int32 CurrentPlayerNum = BaseGameState->PlayerArray.Num();
	// OccupationGameState->SetNumPlayers(CurrentPlayerNum);

	if (CurrentPlayerNum == BaseGameState->GetMaximumPlayers())
	{
		GetWorldTimerManager().SetTimer(TimerHandle_DelayedCharacterSelectStart, this, &ALakayaDefaultPlayGameMode::StartSelectCharacter,
			CharacterSelectStartDelay, false);
	}
}

void ALakayaDefaultPlayGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
	if (MatchState == MatchState::IsSelectCharacter)
	{
		HandleMatchIsSelectCharacter();
	}
}

void ALakayaDefaultPlayGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
}

bool ALakayaDefaultPlayGameMode::ReadyToStartMatch_Implementation()
{
	return Super::ReadyToStartMatch_Implementation();
}

void ALakayaDefaultPlayGameMode::HandleMatchIsSelectCharacter()
{
	FTimerHandle TimerHandler;
	//GetWorldTimerManager().SetTimer(TimerHandler, this, &ALakayaDefaultPlayGameMode::StartMatch, 10.0f, false);
}

void ALakayaDefaultPlayGameMode::HandleMatchHasStarted()
{
	// 게임 시작 후, 서버 측 클라에게 UI바인딩.
	Super::HandleMatchHasStarted();
	
	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleMatchHasStarted"));
}

void ALakayaDefaultPlayGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleMatchHasEnded"));
}

void ALakayaDefaultPlayGameMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();

	// TODO
	UE_LOG(LogTemp, Error, TEXT("HandleLeavingMap"));
}

void ALakayaDefaultPlayGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	UE_LOG(LogTemp, Warning, TEXT("The Player has left the game."));
	UE_LOG(LogTemp, Warning, TEXT("Current Player Num : %d"), NumPlayers);
}

void ALakayaDefaultPlayGameMode::OnPlayerKilled(AController* VictimController, AController* InstigatorController, AActor* DamageCauser)
{
	if (const auto InstigatorPlayerState = InstigatorController->GetPlayerState<ALakayaBasePlayerState>())
		InstigatorPlayerState->IncreaseKillCount();

	const auto VictimPlayerState = VictimController->GetPlayerState<ALakayaBasePlayerState>();
	if (VictimPlayerState != nullptr) VictimPlayerState->IncreaseDeathCount();

	if (const auto BaseGameState = GetGameState<ALakayaBaseGameState>())
		BaseGameState->NotifyPlayerKilled(VictimController, InstigatorController, DamageCauser);

	if (ShouldRespawn())
	{
		VictimPlayerState->SetRespawnTimer(GetGameState<AGameState>()->GetServerWorldTimeSeconds() + MinRespawnDelay, this, &ALakayaDefaultPlayGameMode::RespawnPlayer);
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("RespawnTimerSetted!!!"));
	}
	else
	{
		VictimPlayerState->SetRespawnTimer(-1.0f, this);
	}
} 

void ALakayaDefaultPlayGameMode::StartSelectCharacter()
{
	if (MatchState != MatchState::WaitingToStart) return;

	SetMatchState(MatchState::IsSelectCharacter);
}

void ALakayaDefaultPlayGameMode::DelayedEndedGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainLobbyLevel");
}

bool ALakayaDefaultPlayGameMode::HasMatchStarted() const
{
	if (MatchState == MatchState::IsSelectCharacter) return false;

	return Super::HasMatchStarted();
}

void ALakayaDefaultPlayGameMode::PlayerInitializeSetLocation(uint8 PlayersNum)
{
	// TODO
}

UClass* ALakayaDefaultPlayGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const auto PlayerState = InController->GetPlayerState<ALakayaBasePlayerState>())
		if (CharacterClasses.Contains(PlayerState->GetCharacterName()))
			return CharacterClasses[PlayerState->GetCharacterName()];

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void ALakayaDefaultPlayGameMode::RespawnPlayer(AController* KilledController)
{
	RestartPlayer(KilledController);

	/*GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("RespawnPlayer!!!!"));


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
	*/
	// ADamageableCharacter* KilledDamageableCharacter = Cast<ADamageableCharacter>(KilledCharacterActor);
	// if (KilledDamageableCharacter == nullptr)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("KilledDamageableCharacter is null."));
	// 	return;
	// }
	//
	// KilledDamageableCharacter->Respawn();
}

bool ALakayaDefaultPlayGameMode::ShouldRespawn()
{
	return true;
}
