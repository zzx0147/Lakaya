#include "GameMode/OccupationGameMode.h"
#include "Character/InteractableCharacter.h"
#include "Character/StatPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "GameMode/OccupationGameState.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/InteractablePlayerController.h"

AOccupationGameMode::AOccupationGameMode() : Super()
{
	ScoreUpdateDelay = 0.5f;
	AdditiveScore = 0.1f;
	// MatchStartDelay = 5.f;
	// MatchEndDelay = 2.f;

	DefaultPawnClass = AInteractableCharacter::StaticClass();
	PlayerControllerClass = AInteractablePlayerController::StaticClass();
	PlayerStateClass = AStatPlayerState::StaticClass();
	GameStateClass = AOccupationGameState::StaticClass();
}

void AOccupationGameMode::OnPlayerKilled(AController* VictimController, AController* InstigatorController, AActor* DamageCauser)
{
	Super::OnPlayerKilled(VictimController, InstigatorController, DamageCauser);
	//OccupationGameState->NotifyKillCharacter(VictimController, InstigatorController, DamageCauser);
}

void AOccupationGameMode::BeginPlay()
{
	OccupationGameState = GetGameState<AOccupationGameState>();
}

void AOccupationGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AOccupationGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	// TODO : 플레이어가 나갈 시 나간 플레이어의 정보를 ai에게 넣어줘야함
}

bool AOccupationGameMode::ReadyToStartMatch_Implementation()
{
	if (GetMatchState() != MatchState::WaitingToStart) return false;

	//if (!GetbWaitToStart()) return false;

	return Super::ReadyToStartMatch_Implementation();
}

bool AOccupationGameMode::ReadyToEndMatch_Implementation()
{
	// return OccupationGameState->GetMatchRemainTime() <= 0.f || OccupationGameState->IsSomeoneReachedMaxScore();
	return false;
}

void AOccupationGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	// OccupationGameState->SetMatchTime();

	// 플레이어 인원만큼 위치를 조정해줍니다. (각각의 팀 위치에서)
	//PlayerInitializeSetLocation(OccupationGameState->PlayerArray.Num());

	GetWorldTimerManager().SetTimer(UpdateScoreTimer, this, &AOccupationGameMode::UpdateTeamScoreTick, ScoreUpdateDelay,true);

	UE_LOG(LogTemp, Error, TEXT("HandleMatchHasStarted"));
}

void AOccupationGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	GetWorldTimerManager().ClearTimer(UpdateScoreTimer);
	OccupationGameState->SetOccupationWinner();
	//GetWorldTimerManager().SetTimer(TimerHandle_DelayedEnded, this, &AOccupationGameMode::DelayedEndedGame,
	//                                MatchEndDelay, false);
}

void AOccupationGameMode::HandleMatchIsSelectCharacter()
{
	Super::HandleMatchIsSelectCharacter();
	if (OccupationGameState->GetMaximumPlayers() == OccupationGameState->PlayerArray.Num())
	{
		for (int i = 0; i < OccupationGameState->GetMaximumPlayers(); i++)
		{
			if (OccupationGameState->PlayerArray.IsValidIndex(i))
			{
				auto* LakayaBasePlayerState = Cast<ALakayaBasePlayerState>(OccupationGameState->PlayerArray[i]);
				if (LakayaBasePlayerState == nullptr)
				{
					UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_CollectorPlayerState is null."));
					return;
				}

				if (i % 2 == 0)
				{
					LakayaBasePlayerState->SetTeam(EPlayerTeam::A);
					UE_LOG(LogTemp, Warning, TEXT("A팀에 배정 되었습니다."));
				}
				else
				{
					LakayaBasePlayerState->SetTeam(EPlayerTeam::B);
					UE_LOG(LogTemp, Warning, TEXT("B팀에 배정 되었습니다."));
				}
			}
		}
	}

}

void AOccupationGameMode::UpdateTeamScoreTick()
{
	if (ATeamObjectCount > 0) OccupationGameState->AddTeamScore(EPlayerTeam::A, ATeamObjectCount * AdditiveScore);
	if (BTeamObjectCount > 0) OccupationGameState->AddTeamScore(EPlayerTeam::B, BTeamObjectCount * AdditiveScore);
}

void AOccupationGameMode::RespawnPlayer(AController* KilledController)
{
	Super::RespawnPlayer(KilledController);
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("RespawnPlayer!!!!"));


	//const auto* CollectorPlayerState = Cast<ALakayaBasePlayerState>(KilledController->PlayerState);
	//if (CollectorPlayerState == nullptr)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("LakayaDefaultPlayGameMode_CollectorPlayerState is null."));
	//	return;
	//}

	//FName SpawnTag;
	//switch (CollectorPlayerState->GetTeam())
	//{
	//case EPlayerTeam::A:
	//	SpawnTag = FName("ATeamSpawnZone");
	//	break;
	//case EPlayerTeam::B:
	//	SpawnTag = FName("BTeamSpawnZone");
	//	break;
	//default:
	//	UE_LOG(LogTemp, Warning, TEXT("Invalid player team state."));
	//	return;
	//}

	//UE_LOG(LogTemp, Warning, TEXT("SpawnTag: %s"), *SpawnTag.ToString());

	//TArray<AActor*> PlayerStartActors;
	//UGameplayStatics::GetAllActorsWithTag(GetWorld(), SpawnTag, PlayerStartActors);

	//UE_LOG(LogTemp, Warning, TEXT("PlayerStartActors.Num(): %d"), PlayerStartActors.Num());

	//if (PlayerStartActors.Num() == 0)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("No Player Start Actors found."));
	//	return;
	//}

	//const APlayerStart* PlayerStart = Cast<APlayerStart>(
	//	PlayerStartActors[FMath::RandRange(0, PlayerStartActors.Num() - 1)]);
	//APawn* KilledPawn = Cast<APawn>(KilledController->GetPawn());
	//ACharacter* KilledCharacterActor = Cast<ACharacter>(KilledController->GetCharacter());

	//if (KilledPawn != nullptr)
	//{
	//	KilledPawn->SetActorLocation(PlayerStart->GetActorLocation());
	//}
	//else if (KilledController != nullptr)
	//{
	//	KilledCharacterActor->SetActorLocation(PlayerStart->GetActorLocation());
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_KilledCharacter is not a pawn or an actor."));
	//	return;
	//}

	// auto* KilledDamageableCharacter = Cast<ADamageableCharacter>(KilledCharacterActor);
	// if (KilledDamageableCharacter == nullptr)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("KilledDamageableCharacter is null."));
	// 	return;
	// }
	//
	// KilledDamageableCharacter->Respawn();
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
