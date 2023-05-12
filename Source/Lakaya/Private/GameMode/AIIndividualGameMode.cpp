#include "GameMode/AIIndividualGameMode.h"

#include "Character/ArmedCharacter.h"
#include "Character/StatPlayerState.h"
#include "GameMode/AIIndividualGameState.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/BattlePlayerController.h"

AAIIndividualGameMode::AAIIndividualGameMode()
{
	// MatchStartDelay = 5.0f;
	// MatchEndDelay = 2.0f;
	
	DefaultPawnClass = AArmedCharacter::StaticClass();
	PlayerControllerClass = ABattlePlayerController::StaticClass();
	PlayerStateClass = AStatPlayerState::StaticClass();
	GameStateClass = AAIIndividualGameState::StaticClass();
}

void AAIIndividualGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AIIndividualGameState = GetWorld()->GetGameState<AAIIndividualGameState>();
	if (AIIndividualGameState == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIIndividualGameMode_AIIndividualGameState is null."));
		return;
	}

	GetWorldTimerManager().SetTimer(TimerHandle_DelayedStart, this, &ALakayaDefaultPlayGameMode::StartSelectCharacter, MatchStartDelay, false);
}

bool AAIIndividualGameMode::ReadyToStartMatch_Implementation()
{
	if (GetMatchState() != MatchState::WaitingToStart)return false;

	if (!GetbWaitToStart()) return false;

	return true;
}

bool AAIIndividualGameMode::ReadyToEndMatch_Implementation()
{
	return false;	
}

void AAIIndividualGameMode::HandleMatchIsSelectCharacter()
{
	Super::HandleMatchIsSelectCharacter();

	// 팀 배정 (개인전)
	if (AIIndividualGameState->PlayerArray.IsValidIndex(0))
	{
		auto* PlayerState = Cast<ALakayaBasePlayerState>(AIIndividualGameState->PlayerArray[0]);
		if (PlayerState == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("AIIndividualGameMode_PlayerState is null."));
			return;
		}

		PlayerState->SetTeam(EPlayerTeam::Individual);
	}
}

void AAIIndividualGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	// TODO : 플레이어와 AI 위치를 조정
	// Code
	UE_LOG(LogTemp, Warning, TEXT("HandleMatchHasStarted."));
	
	// auto* AIIndividualGameMode = Cast<AAIIndividualGameMode>(GetWorld()->GetAuthGameMode());
	// if (AIIndividualGameMode == nullptr)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("AIIndividualGameMode_AIIndividualGameMode is null."));
	// 	return;
	// }
}

void AAIIndividualGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	// TODO : 1등 플레이어 (AI) 지정
	//
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedEnded, this, &AAIIndividualGameMode::DelayedEndedGame,
		MatchEndDelay, false);
}

void AAIIndividualGameMode::DelayedEndedGame()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainLobbyLevel");
}