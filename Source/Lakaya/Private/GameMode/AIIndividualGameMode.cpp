#include "GameMode/AIIndividualGameMode.h"

#include "Character/StatPlayerState.h"
#include "GameMode/AIIndividualGameState.h"
#include "Kismet/GameplayStatics.h"
#include "AI/AiCharacterController.h"

AAIIndividualGameMode::AAIIndividualGameMode()
{
	// MatchStartDelay = 5.0f;
	// MatchEndDelay = 2.0f;
	
	// DefaultPawnClass = AArmedCharacter::StaticClass();
	// PlayerControllerClass = ABattlePlayerController::StaticClass();
	// PlayerStateClass = AStatPlayerState::StaticClass();
	// GameStateClass = AAIIndividualGameState::StaticClass();

	NumberOfAi = 5;
	TargetKills = 20;
	WinningCharController = nullptr;
}

void AAIIndividualGameMode::BeginPlay()
{
	Super::BeginPlay();

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		PlayerController = It->Get();
		if (PlayerController)
		{
			ALakayaBasePlayerState* PlayerAIState = Cast<ALakayaBasePlayerState>(PlayerController->PlayerState);

			if (PlayerAIState)
			{
				PlayerAIState->OnKillCountChanged.AddLambda([this](const uint16& NewKillCount)
				{
					HandleKillCountChanged(NewKillCount);
				});
			}
		}
	}

}

void AAIIndividualGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (AiControllerArray.Num() > 0) return;

	for (int i = 0; i < NumberOfAi; ++i) //첫번째 플레이어가 접속하면 그때 AI를 생성합니다, 플레이어가 한명일 때만을 가정합니다
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

	AAIIndividualGameState* AiIndividualGameState = GetGameState<AAIIndividualGameState>();
	if (AiIndividualGameState)
	{
		AiIndividualGameState->SetAIIndividualWinner();
	}
	GetWorldTimerManager().SetTimer(TimerHandle_DelayedEnded, this, &AAIIndividualGameMode::DelayedEndedGame,
									MatchEndDelay, false);
	
	UE_LOG(LogTemp, Warning, TEXT("AI HandleMatchHasEnded was called"));
}

void AAIIndividualGameMode::HandleKillCountChanged(const uint16& NewKillCount)
{
	if (NewKillCount >= TargetKills)
	{
		EndGame(WinningCharController);
		UE_LOG(LogTemp, Warning, TEXT("최고 킬 달성"));
	}
}

void AAIIndividualGameMode::EndGame(AController* WinerController)
{
	// TODO : 이긴 플레이어나 AI를 가져와 게임 종료시 점수판에 보여주고 메인화면으로 돌아가는 함수.
	
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainLobbyLevel"));
}
