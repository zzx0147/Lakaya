#include "GameMode/AIIndividualGameMode.h"

#include "Character/ArmedCharacter.h"
#include "Character/StatPlayerState.h"
#include "GameMode/AIIndividualGameState.h"
#include "Kismet/GameplayStatics.h"
#include "AI/AiCharacterController.h"
#include "PlayerController/BattlePlayerController.h"

AAIIndividualGameMode::AAIIndividualGameMode()
{
	// MatchStartDelay = 5.0f;
	// MatchEndDelay = 2.0f;
	
	// DefaultPawnClass = AArmedCharacter::StaticClass();
	// PlayerControllerClass = ABattlePlayerController::StaticClass();
	// PlayerStateClass = AStatPlayerState::StaticClass();
	// GameStateClass = AAIIndividualGameState::StaticClass();

	NumberOfAi = 5;
	TargetKills = 5;
}

void AAIIndividualGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (AiControllerArray.Num() > 0) return;

	for (int i = 0; i < NumberOfAi; ++i)//첫번째 플레이어가 접속하면 그때 AI를 생성합니다, 플레이어가 한명일 때만을 가정합니다
	{
		AAiCharacterController* AiController;
		AiController = GetWorld()->SpawnActor<AAiCharacterController>(AIControllerClass);
		AiControllerArray.Emplace(AiController);
		RegisterPlayer(AiController);
	}
}

void AAIIndividualGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	for(const auto AiController : AiControllerArray)
	{
		RestartPlayer(AiController);
	}	
}

void AAIIndividualGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	// TODO : 1등 플레이어 (AI) 지정

	bool TargetKillsAchieved = false;
	AController* WinningCharController = nullptr;

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AController* PlayerController = It->Get();
		if (PlayerController)
		{
			ALakayaBasePlayerState* PlayerAIState = Cast<ALakayaBasePlayerState>(PlayerController->PlayerState);
			if (PlayerAIState && PlayerAIState->GetKillCount() >= TargetKills)
			{
				TargetKillsAchieved = true;
				WinningCharController = PlayerController;
				UE_LOG(LogTemp, Warning, TEXT("최고 킬 달성"));
				break;
			}
		}
	}
	// for (const auto AiController : AiControllerArray)
	// {
	// 	ALakayaBasePlayerState* BasePlayerAIState = Cast<ALakayaBasePlayerState>(AiController->PlayerState);
	// 	if (BasePlayerAIState && BasePlayerAIState->GetKillCount() >= TargetKills)
	// 	{
	// 		TargetKillsAchieved = true;
	// 		WinningCharController = AiController;
	// 		UE_LOG(LogTemp, Warning, TEXT("최고 킬 달성"));
	// 		break;
	// 	}
	// }

	if (TargetKillsAchieved)
	{
		// TODO : 최고점수를 기록한 플레이어나 AI 를 넣어줘야합니다.
		EndGame(WinningCharController);
		UE_LOG(LogTemp, Warning, TEXT("Ended called"));
	}
	else
	{
		// GetWorldTimerManager().ClearTimer(UpdateScoreTimer);
		AAIIndividualGameState* AiIndividualGameState = GetGameState<AAIIndividualGameState>();
		if (AiIndividualGameState)
		{
			AiIndividualGameState->SetAIIndividualWinner();
		}
		GetWorldTimerManager().SetTimer(TimerHandle_DelayedEnded, this, &AAIIndividualGameMode::DelayedEndedGame,
										MatchEndDelay, false);
	}

	UE_LOG(LogTemp, Warning, TEXT("AI HandleMatchHasEnded was called"));
}

void AAIIndividualGameMode::EndGame(AController* WinningCharController)
{
	// TODO : 이긴 플레이어나 AI를 가져와 게임 종료시 점수판에 보여주고 메인화면으로 돌아가는 함수.
	
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainLobbyLevel"));
}

// void AAIIndividualGameMode::PostLogin(APlayerController* NewPlayer)
// {
// 	Super::PostLogin(NewPlayer);
//
// 	AIIndividualGameState = GetWorld()->GetGameState<AAIIndividualGameState>();
// 	if (AIIndividualGameState == nullptr)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("AIIndividualGameMode_AIIndividualGameState is null."));
// 		return;
// 	}
//
// 	GetWorldTimerManager().SetTimer(TimerHandle_DelayedStart, this, &AAIIndividualGameMode::StartSelectCharacter, MatchStartDelay, false);
// }
//
// bool AAIIndividualGameMode::ReadyToStartMatch_Implementation()
// {
// 	if (GetMatchState() != MatchState::WaitingToStart)return false;
//
// 	if (!GetbWaitToStart()) return false;
//
// 	return true;
// }
//
// bool AAIIndividualGameMode::ReadyToEndMatch_Implementation()
// {
// 	return false;	
// }
//
// void AAIIndividualGameMode::HandleMatchIsSelectCharacter()
// {
// 	Super::HandleMatchIsSelectCharacter();
//
// 	// 팀 배정 (개인전)
// 	if (AIIndividualGameState->PlayerArray.IsValidIndex(0))
// 	{
// 		auto* PlayerState = Cast<ALakayaBasePlayerState>(AIIndividualGameState->PlayerArray[0]);
// 		if (PlayerState == nullptr)
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("AIIndividualGameMode_PlayerState is null."));
// 			return;
// 		}
//
// 		PlayerState->SetTeam(EPlayerTeam::Individual);
// 	}
// }
//
// void AAIIndividualGameMode::HandleMatchHasStarted()
// {
// 	Super::HandleMatchHasStarted();
//
// 	// TODO : 플레이어와 AI 위치를 조정
// 	// Code
// 	UE_LOG(LogTemp, Warning, TEXT("HandleMatchHasStarted."));
// 	
// 	// auto* AIIndividualGameMode = Cast<AAIIndividualGameMode>(GetWorld()->GetAuthGameMode());
// 	// if (AIIndividualGameMode == nullptr)
// 	// {
// 	// 	UE_LOG(LogTemp, Warning, TEXT("AIIndividualGameMode_AIIndividualGameMode is null."));
// 	// 	return;
// 	// }
// }
//
// void AAIIndividualGameMode::RespawnPlayer(AController* KilledController)
// {
// 	Super::RespawnPlayer(KilledController);
// }
//
// void AAIIndividualGameMode::PlayerInitializeSetLocation(uint8 PlayersNum)
// {
// 	Super::PlayerInitializeSetLocation(PlayersNum);
// }