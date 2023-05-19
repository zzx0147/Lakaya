#include "GameMode/OccupationGameMode.h"
#include "Character/InteractableCharacter.h"
#include "Character/StatPlayerState.h"
#include "GameMode/OccupationGameState.h"
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

//TODO: 필요없는 함수 오버라이딩
void AOccupationGameMode::OnPlayerKilled(AController* VictimController, AController* InstigatorController, AActor* DamageCauser)
{
	Super::OnPlayerKilled(VictimController, InstigatorController, DamageCauser);
	//OccupationGameState->NotifyKillCharacter(VictimController, InstigatorController, DamageCauser);
}

void AOccupationGameMode::BeginPlay()
{
	OccupationGameState = GetGameState<AOccupationGameState>();


}

//TODO: 필요없는 함수 오버라이딩
void AOccupationGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AOccupationGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	// TODO : 플레이어가 나갈 시 나간 플레이어의 정보를 ai에게 넣어줘야함
}

//TODO: Tick을 비활성화해두고 매치스테이트를 직접 제어하는 방식으로 구조를 전환했으므로 더이상 사용되지 않습니다.
bool AOccupationGameMode::ReadyToStartMatch_Implementation()
{
	if (GetMatchState() != MatchState::WaitingToStart) return false;

	//if (!GetbWaitToStart()) return false;

	return Super::ReadyToStartMatch_Implementation();
}

//TODO: Tick을 비활성화해두고 매치스테이트를 직접 제어하는 방식으로 구조를 전환했으므로 더이상 사용되지 않습니다.
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
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedEnded, this, &AOccupationGameMode::DelayedEndedGame,
	                                MatchEndDelay, false);
	UE_LOG(LogTemp, Warning, TEXT("시발 HandleMatchHasEnded가 떳어요"));
}

void AOccupationGameMode::HandleMatchIsSelectCharacter()
{
	Super::HandleMatchIsSelectCharacter();
	//TODO: 비교연산을 수행하는 두 변수 모두 OccupationGameState의 멤버변수이므로, OccupationGameState에 함수를 만들어서 플레이어 숫자가 가득 찼는지 여부를 조사하는 편이 좋을 것 같습니다. 
	if (OccupationGameState->GetMaximumPlayers() == OccupationGameState->PlayerArray.Num())
	{
		//TODO: 이렇게 사용하면 더 간단히 표현할 수 있습니다.
		// uint8 Count = 0;
		// for (auto& LakayaBasePlayerState : OccupationGameState->PlayerArray)
		// {
		// 	++Count;
		// 	if (!LakayaBasePlayerState) continue;
		// 	
		// }
		for (int i = 0; i < OccupationGameState->GetMaximumPlayers(); i++)
		{
			if (OccupationGameState->PlayerArray.IsValidIndex(i))
			{
				auto* LakayaBasePlayerState = Cast<ALakayaBasePlayerState>(OccupationGameState->PlayerArray[i]);
				if (LakayaBasePlayerState == nullptr)
				{
					UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_CollectorPlayerState is null."));
					//TODO: 리턴을 해버리면 다른 정상적인 플레이어 스테이트는 팀이 배정되지 않고 넘어갑니다. 차라리 Fatal 로그를 사용하여 게임을 튕겨버리도록 하거나, continue를 하는 편이 낫습니다.
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
				//TODO: 위의 분기문은 다음과 같이 축약 가능
				// const auto Team = i % 2 == 0 ? EPlayerTeam::A : EPlayerTeam::B;
				// LakayaBasePlayerState->SetTeam(Team);
				// UE_LOG(LogNet, Log, TEXT("%d 팀에 배정 되었습니다."), Team);
			}
		}
	}
}

void AOccupationGameMode::UpdateTeamScoreTick()
{
	if (ATeamObjectCount > 0) OccupationGameState->AddTeamScore(EPlayerTeam::A, ATeamObjectCount * AdditiveScore);
	if (BTeamObjectCount > 0) OccupationGameState->AddTeamScore(EPlayerTeam::B, BTeamObjectCount * AdditiveScore);
}

//TODO: 필요없는 함수 오버라이딩
void AOccupationGameMode::RespawnPlayer(AController* KilledController)
{
	Super::RespawnPlayer(KilledController);
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("RespawnPlayer!!!!"));

#pragma region 주석
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
#pragma endregion
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
	else UE_LOG(LogScript, Warning, TEXT("Trying to SubOccupyObject with invalid value! it was %d"), Team);
}