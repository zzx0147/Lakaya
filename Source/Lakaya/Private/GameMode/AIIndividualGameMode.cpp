#include "GameMode/AIIndividualGameMode.h"

#include <random>

#include "GameMode/AIIndividualGameState.h"
#include "Kismet/GameplayStatics.h"
#include "AI/AiCharacterController.h"

AAIIndividualGameMode::AAIIndividualGameMode()
{
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

	std::random_device rd; // 랜덤 디바이스 초기화
	std::mt19937 gen(rd()); // 랜덤 엔진 초기화
	std::uniform_int_distribution<> distrib(0, 1); // 해당 범위 정수 중 임의의 값 생성
	
	for (int i = 0; i < NumberOfAi; ++i) //첫번째 플레이어가 접속하면 그때 AI를 생성합니다, 플레이어가 한명일 때만을 가정합니다
	{
		AAiCharacterController* AiController;
		AiController = GetWorld()->SpawnActor<AAiCharacterController>(AIControllerClass);

		// 랜덤 캐릭터 이름 생성
		FName RandomCharacterName = (distrib(gen) == 0) ? TEXT("Rena") : TEXT("Wazi");

		// AI의 캐릭터 변경
		ALakayaBasePlayerState* AiState = AiController->GetPlayerState<ALakayaBasePlayerState>();
		AiState->RequestCharacterChange(RandomCharacterName);
		
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
		// EndGame(WinningCharController);
		UE_LOG(LogTemp, Warning, TEXT("최고 킬 달성"));
		EndMatch();
	}
}