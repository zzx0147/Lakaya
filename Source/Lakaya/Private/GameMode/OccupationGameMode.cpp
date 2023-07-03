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

	GetWorldTimerManager().SetTimer(UpdateScoreTimer, this, &AOccupationGameMode::UpdateTeamScoreTick, ScoreUpdateDelay,true);

	UE_LOG(LogTemp, Error, TEXT("HandleMatchHasStarted"));
}

void AOccupationGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	GetWorldTimerManager().ClearTimer(UpdateScoreTimer);
	OccupationGameState->SetOccupationWinner();

	// TODO : 이제는 게임이 끝나게 되면 자동으로 로비창으로 이동하는 것이 아닌, 플레이어의 입력에 따라 로비창으로 이동합니다.
	// GetWorldTimerManager().SetTimer(TimerHandle_DelayedEnded, this, &AOccupationGameMode::DelayedEndedGame,
	//                                 MatchEndDelay, false);
}

void AOccupationGameMode::HandleMatchIsSelectCharacter()
{
	Super::HandleMatchIsSelectCharacter();
	//TODO: 비교연산을 수행하는 두 변수 모두 OccupationGameState의 멤버변수이므로, OccupationGameState에 함수를 만들어서 플레이어 숫자가 가득 찼는지 여부를 조사하는 편이 좋을 것 같습니다. 
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
				}

				const auto Team = i % 2 == 0 ? EPlayerTeam::A : EPlayerTeam::B;
				LakayaBasePlayerState->SetTeam(Team);
				UE_LOG(LogTemp, Warning, TEXT("%s"), i % 2 == 0 ? TEXT("A팀에 배정 되었습니다.") : TEXT("B팀에 배정 되었습니다."));
			}
		}
	}
}

void AOccupationGameMode::RespawnPlayer(AController* KilledController)
{
	Super::RespawnPlayer(KilledController);
}

void AOccupationGameMode::UpdateTeamScoreTick()
{
	if (ATeamObjectCount > 0) OccupationGameState->AddTeamScore(EPlayerTeam::A, ATeamObjectCount * AdditiveScore);
	if (BTeamObjectCount > 0) OccupationGameState->AddTeamScore(EPlayerTeam::B, BTeamObjectCount * AdditiveScore);
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