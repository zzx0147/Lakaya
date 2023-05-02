// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/OccupationGameMode.h"

#include "Character/ArmedCharacter.h"
#include "Character/OccupationCharacter.h"
#include "Character/OccupationPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "GameMode/OccupationGameState.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/OccupationPlayerController.h"

AOccupationGameMode::AOccupationGameMode()
{
	ScoreUpdateDelay = 0.5f;
	AdditiveScore = 0.1f;
	MatchStartDelay = 5.f;
	MatchEndDelay = 2.f;

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnObject(
		TEXT("/Game/Characters/LakayaCharacter/Dummy/BP_PlayerDummy"));
	if (!PlayerPawnObject.Succeeded())
		UE_LOG(LogTemp, Error, TEXT("OccupationGameMode_Failed to find player pawn blueprint."));

	DefaultPawnClass = AOccupationCharacter::StaticClass();
	PlayerControllerClass = AOccupationPlayerController::StaticClass();
	PlayerStateClass = AOccupationPlayerState::StaticClass();
	GameStateClass = AOccupationGameState::StaticClass();
}

void AOccupationGameMode::OnKilledCharacter(AController* VictimController, AActor* Victim,
                                            AController* InstigatorController, AActor* DamageCauser)
{
	Super::OnKilledCharacter(VictimController, Victim, InstigatorController, DamageCauser);
	OccupationGameState->NotifyKillCharacter(VictimController, Victim, InstigatorController, DamageCauser);
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

	const int32 CurrentPlayerNum = OccupationGameState->PlayerArray.Num();
	OccupationGameState->SetNumPlayers(CurrentPlayerNum);

	if (GetNumPlayers() >= OccupationGameState->GetMaximumPlayers())
	{
		GetWorldTimerManager().SetTimer(TimerHandle_DelayedStart, this, &ALakayaDefaultPlayGameMode::StartSelectCharacter,
		                                MatchStartDelay, false);
	}
}

bool AOccupationGameMode::ReadyToStartMatch_Implementation()
{
	if (GetMatchState() != MatchState::WaitingToStart) return false;

	if (!GetbWaitToStart()) return false;

	for (int i = 0; i < OccupationGameState->GetMaximumPlayers(); i++)
	{
		if (OccupationGameState->PlayerArray.IsValidIndex(i))
		{
			AOccupationPlayerState* CollectorPlayerState = Cast<AOccupationPlayerState>(
				OccupationGameState->PlayerArray[i]);
			if (CollectorPlayerState == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_CollectorPlayerState is null."));
				return false;
			}

			if (i % 2 == 0)
			{
				CollectorPlayerState->SetPlayerTeamState(EPlayerTeam::A);
				UE_LOG(LogTemp, Warning, TEXT("A팀에 배정 되었습니다."));
			}
			else
			{
				CollectorPlayerState->SetPlayerTeamState(EPlayerTeam::B);
				UE_LOG(LogTemp, Warning, TEXT("B팀에 배정 되었습니다."));
			}
		}
	}

	return true;
}

bool AOccupationGameMode::ReadyToEndMatch_Implementation()
{
	return OccupationGameState->GetRemainMatchTime() <= 0.f || OccupationGameState->IsSomeoneReachedMaxScore();
}

void AOccupationGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	OccupationGameState->SetMatchTime();

	// 플레이어 인원만큼 위치를 조정해줍니다. (각각의 팀 위치에서)
	PlayerInitializeSetLocation(OccupationGameState->PlayerArray.Num());

	GetWorldTimerManager().SetTimer(UpdateScoreTimer, this, &AOccupationGameMode::UpdateTeamScoreTick, ScoreUpdateDelay,
	                                true);

	UE_LOG(LogTemp, Error, TEXT("HandleMatchHasStarted"));
}

void AOccupationGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	GetWorldTimerManager().ClearTimer(UpdateScoreTimer);
	OccupationGameState->SetOccupationWinner();
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedEnded, this, &AOccupationGameMode::DelayedEndedGame,
	                                MatchEndDelay, false);
}

void AOccupationGameMode::DelayedEndedGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainLobbyLevel");
}

void AOccupationGameMode::UpdateTeamScoreTick()
{
	if (ATeamObjectCount > 0) OccupationGameState->AddTeamScore(EPlayerTeam::A, ATeamObjectCount * AdditiveScore);
	if (BTeamObjectCount > 0) OccupationGameState->AddTeamScore(EPlayerTeam::B, BTeamObjectCount * AdditiveScore);
}

void AOccupationGameMode::RespawnPlayer(AController* KilledController)
{
	const AOccupationPlayerState* CollectorPlayerState = Cast<AOccupationPlayerState>(KilledController->PlayerState);
	if (CollectorPlayerState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("LakayaDefaultPlayGameMode_CollectorPlayerState is null."));
		return;
	}

	FName SpawnTag;
	switch (CollectorPlayerState->GetPlayerTeamState())
	{
	case EPlayerTeam::A:
		SpawnTag = FName("ATeamSpawnZone");
		break;
	case EPlayerTeam::B:
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

	const APlayerStart* PlayerStart = Cast<APlayerStart>(
		PlayerStartActors[FMath::RandRange(0, PlayerStartActors.Num() - 1)]);
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

void AOccupationGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	if (const auto Character = NewPlayer->GetPawn<ALakayaBaseCharacter>())
		Character->SetupCharacter(TEXT("Test"));
	Super::FinishRestartPlayer(NewPlayer, StartRotation);
}

void AOccupationGameMode::AddOccupyObject(const EPlayerTeam& Team)
{
	if (Team == EPlayerTeam::A) ++ATeamObjectCount;
	else if (Team == EPlayerTeam::B) ++BTeamObjectCount;
	else UE_LOG(LogScript, Warning, TEXT("Trying to AddOccupyObject with invalid value! it was %d"), Team);
}

void AOccupationGameMode::SubOccupyObject(const EPlayerTeam& Team)
{
	if (Team == EPlayerTeam::A && ATeamObjectCount > 0) --ATeamObjectCount;
	else if (Team == EPlayerTeam::B && BTeamObjectCount > 0) --BTeamObjectCount;
	else UE_LOG(LogScript, Warning, TEXT("Trying to AddOccupyObject with invalid value! it was %d"), Team);
}

void AOccupationGameMode::PlayerInitializeSetLocation(uint8 PlayersNum)
{
	for (int i = 0; PlayersNum; i++)
	{
		if (OccupationGameState->PlayerArray.IsValidIndex(i))
		{
			const AOccupationPlayerState* CollectorPlayerState = Cast<AOccupationPlayerState>(
				OccupationGameState->PlayerArray[i]);
			if (CollectorPlayerState == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_CollectorPlayerState is null."));
				return;
			}

			AController* OccuController = Cast<AController>(CollectorPlayerState->GetOwner());
			if (OccuController == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_PlayerController is null."));
				return;
			}

			FName SpawnTag;
			switch (CollectorPlayerState->GetPlayerTeamState())
			{
			case EPlayerTeam::A:
				SpawnTag = FName("ATeamSpawnZone");
				break;
			case EPlayerTeam::B:
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

			const APlayerStart* PlayerStart = Cast<APlayerStart>(
				PlayerStartActors[FMath::RandRange(0, PlayerStartActors.Num() - 1)]);
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
