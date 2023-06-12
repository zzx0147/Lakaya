#include "GameMode/AIIndividualGameState.h"

#include "AI/AiCharacterController.h"
#include "Character/LakayaBasePlayerState.h"
#include "ETC/OutlineManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerController/InteractablePlayerController.h"
#include "UI/IndividualWidget/IndividualGameResultWidget.h"
#include "UI/IndividualWidget/IndividualLiveScoreBoardWidget.h"

AAIIndividualGameState::AAIIndividualGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	MatchStartWaitWidgetLifeTime = 3.0f;

	static ConstructorHelpers::FClassFinder<UIndividualLiveScoreBoardWidget> AIIndividualLiveScoreBoardFinder(
		TEXT("/Game/Blueprints/UMG/IndividualWidget/WBP_IndividualLiveScoreBoardWidget"));

	AIIndividualLiveScoreBoardWidgetClass = AIIndividualLiveScoreBoardFinder.Class;
}

void AAIIndividualGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	if (const auto CastedState = Cast<ALakayaBasePlayerState>(PlayerState))
	{
		auto OnOwnerChanged = [this](AActor* InOwner, ALakayaBasePlayerState* State, const uint8 Count)
		{
			const auto Controller = Cast<APlayerController>(InOwner);
			State->SetAlly(Controller && Controller->IsLocalController());
			State->SetUniqueStencilMask(GetUniqueStencilMaskWithCount(Count));
		};

		const auto Count = PlayerArray.Num();
		OnOwnerChanged(CastedState->GetOwner(), CastedState, Count);
		CastedState->OnOwnerChanged.AddLambda(OnOwnerChanged, CastedState, Count);
	}
}

void AAIIndividualGameState::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnOutlineManager()) OutlineManager->SetTeam(EPlayerTeam::Individual);

	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		AIIndividualLiveScoreBoardWidget = CreateWidget<UIndividualLiveScoreBoardWidget>(
			LocalController, AIIndividualLiveScoreBoardWidgetClass);

		if (GameResultWidgetClass)
		{
			GameResultWidget = CreateWidget<UIndividualGameResultWidget>(LocalController, GameResultWidgetClass);
			if (GameResultWidget == nullptr) UE_LOG(LogTemp, Warning, TEXT("GameResultWidget is null."));
			GameResultWidget->AddToViewport(1);
			GameResultWidget->SetVisibility(ESlateVisibility::Hidden);
		}
		else UE_LOG(LogTemp, Warning, TEXT("GameResultWidgeTClass is null"));
	}
	
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AController* AllControllers = It->Get();

		const auto IndividualPlayerState = Cast<ALakayaBasePlayerState>(
			AllControllers->GetPlayerState<ALakayaBasePlayerState>());

		// AI와 플레이어들의 정보를 Array에 담습니다.
		AllPlayersArray.Add(IndividualPlayerState);

		if (AllControllers && AIIndividualLiveScoreBoardWidget.IsValid())
		{
			AIIndividualLiveScoreBoardWidget->AddToViewport();
			AIIndividualLiveScoreBoardWidget->SetVisibility(ESlateVisibility::Hidden);
			
			ALakayaBasePlayerState* PlayerStateObj = Cast<ALakayaBasePlayerState>(AllControllers->PlayerState);

			// 매치시작전에 스폰된 플레이어와 AI의 정보를 미리 한번 넣어주는곳입니다. 이후 Tick에서 실시간으로 계속 검사합니다.
			if (AllControllers && AllControllers->IsA<AInteractablePlayerController>())
			{
				if (PlayerStateObj)
				{
					// 점수판에 표시되는 플레이어의 이름을 표시합니다.
					PlayerAIData.PlayerName = PlayerStateObj->GetPlayerName();
					PlayerAIData.KillCount = PlayerStateObj->GetKillCount();
					PlayerAIData.bIsPlayerCheck = true;
					FPlayerAIDataArray.Add(PlayerAIData);
				}
			}
			if (AllControllers && !AllControllers->IsA<AInteractablePlayerController>())
			{
				if (PlayerStateObj)
				{
					// 점수판에 표시되는 이름이며 현재 AI 의 캐릭터 + 몃번째의 AI 컨트롤러 번호인지를 표시해주고있습니다.
					AIName = PlayerStateObj->GetCharacterName().ToString()
					+ " AI (" +  FString::FromInt(static_cast<int>(It.GetIndex())) + ")";

					// AI 이름을 Set해주어서 이제 AI도 자기 이름을 가지고 있도록 했습니다.
					PlayerStateObj->SetPlayerName(AIName);
					PlayerAIData.PlayerName = PlayerStateObj->GetPlayerName();
					
					PlayerAIData.KillCount = PlayerStateObj->GetKillCount();
					PlayerAIData.bIsPlayerCheck = false;
					FPlayerAIDataArray.Add(PlayerAIData);
				}
			}
			SetScoreBoardPlayerAIName(FPlayerAIDataArray);
		}
	}
}

void AAIIndividualGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// TODO : 실시간으로 순위판에 계속 킬수를 검사해 위젯 칸에 넣어주기위해 Tick에 구현했습니다 나중에 리팩토링 필요합니다.
	if (AIIndividualLiveScoreBoardWidget.IsValid())
	{
		FPlayerAIDataArray.Empty();

		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			AController* AllControllers = It->Get();
			ALakayaBasePlayerState* PlayerStateObj = Cast<ALakayaBasePlayerState>(AllControllers->PlayerState);

			if (AllControllers && AllControllers->IsA<AInteractablePlayerController>())
			{
				if (PlayerStateObj)
				{
					// 점수판에 표시되는 플레이어의 이름을 표시합니다.
					PlayerAIData.PlayerName = PlayerStateObj->GetPlayerName();
					PlayerAIData.KillCount = PlayerStateObj->GetKillCount();
					PlayerAIData.bIsPlayerCheck = true;
					FPlayerAIDataArray.Add(PlayerAIData);
				}
			}
			if (AllControllers && !AllControllers->IsA<AInteractablePlayerController>())
			{
				if (PlayerStateObj)
				{
					PlayerAIData.PlayerName = PlayerStateObj->GetPlayerName();
					PlayerAIData.KillCount = PlayerStateObj->GetKillCount();
					PlayerAIData.bIsPlayerCheck = false;
					FPlayerAIDataArray.Add(PlayerAIData);
				}
			}
		}

		SetScoreBoardPlayerAIName(FPlayerAIDataArray);
	}
}

void AAIIndividualGameState::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	// 매치 시작하면 플레이어 인풋 막기
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		// LocalController->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_None);
		
		LocalController->DisableInput(LocalController);
	}
	
	FTimerDelegate TimerDelegate;
	GetWorldTimerManager().SetTimer(TimerHandle_WaitTimerHandle, TimerDelegate, MatchStartWaitWidgetLifeTime, false);

	// 게임이 본격적으로 시작이 되면 AI 의 비헤이비어 트리를 시작시켜줍니다.
	TimerDelegate.BindLambda([this]
	{
		if (this == nullptr) return;

		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			AController* AllControllers = It->Get();

			APlayerController* PlayerCharacterController = Cast<APlayerController>(AllControllers);
			AAiCharacterController* AiCharacterController = Cast<AAiCharacterController>(AllControllers);

			if (AllControllers && AIIndividualLiveScoreBoardWidget.IsValid())
				AIIndividualLiveScoreBoardWidget->SetVisibility(ESlateVisibility::Visible);

			// 플레이어 인풋 동작 
			if (PlayerCharacterController)
			{
				// PlayerCharacterController->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
				
				PlayerCharacterController->EnableInput(PlayerCharacterController);
			}

			// AI 동작
			if (AiCharacterController)
			{
				AiCharacterController->BlackboardComp->InitializeBlackboard(*(AiCharacterController->BehaviorTreeAsset->BlackboardAsset));
				AiCharacterController->BehaviorTreeComp->StartTree(*(AiCharacterController->BehaviorTreeAsset));
				AiCharacterController->bIsBehaviorTreeStart = true;
			}
		}
	});
	GetWorldTimerManager().SetTimer(TimerHandle_StartMessageVisible, TimerDelegate, MatchWaitDuration, false);
}

void AAIIndividualGameState::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	if (GameResultWidget.IsValid())
		GameResultWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// 플레이어의 정보를 가져와 게임결과 위젯을 바인딩해줍니다.
	if (const auto LocalPlayerController = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		const auto IndividualPlayerState = Cast<ALakayaBasePlayerState>(
			LocalPlayerController->GetPlayerState<ALakayaBasePlayerState>());
		if (IndividualPlayerState == nullptr) UE_LOG(LogTemp, Warning, TEXT("IndividualPlayerState is null."));

		GameResultWidget->SetScore(IndividualPlayerState->GetTotalScore());
		GameResultWidget->SetKill(IndividualPlayerState->GetKillCount());
		GameResultWidget->SetDeath(IndividualPlayerState->GetDeathCount());
	}

	// for (int i = 0; i < AllPlayersArray.Num(); ++i)
	// {
	// 	AllPlayersArray.Sort([](const TWeakObjectPtr<ALakayaBasePlayerState>& A,
	// 	                        const TWeakObjectPtr<ALakayaBasePlayerState>& B)
	// 	{
	// 		return A->GetTotalScore() > B->GetTotalScore();
	// 	});
	// }

	TArray<FPlayerAIData> SortedPlayerAIDataArray = FPlayerAIDataArray;
	SortedPlayerAIDataArray.Sort([](const FPlayerAIData& A,
	                                const FPlayerAIData& B)
	{
		return A.KillCount > B.KillCount;
	});

	for (int32 i = 0; i < SortedPlayerAIDataArray.Num(); ++i)
	{
		const FPlayerAIData& SortedPlayerAIData = SortedPlayerAIDataArray[i];

		if (SortedPlayerAIData.bIsPlayerCheck == true)
		{
			switch (i)
			{
			case 0:
				GameResultWidget->First_Rank_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				break;
			case 1:
				GameResultWidget->Second_Rank_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				break;
			case 2:
				GameResultWidget->Third_Rank_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				break;
			case 3:
				GameResultWidget->Fourth_Rank_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				break;
			case 4:
				GameResultWidget->Fifth_Rank_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				break;
			case 5:
				GameResultWidget->Sixth_Rank_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				break;
			default:
				break;
			}
		}
	}

	FTimerManager Timers;
	Timers.ClearAllTimersForObject(GetWorld());
}

ERendererStencilMask AAIIndividualGameState::GetUniqueStencilMaskWithCount(const uint8& Count)
{
	switch (Count)
	{
	case 1: return ERendererStencilMask::ERSM_1;
	case 2: return ERendererStencilMask::ERSM_2;
	case 3: return ERendererStencilMask::ERSM_4;
	case 4: return ERendererStencilMask::ERSM_8;
	case 5: return ERendererStencilMask::ERSM_16;
	case 6: return ERendererStencilMask::ERSM_32;
	default: return ERendererStencilMask::ERSM_Default;
	}
}

void AAIIndividualGameState::SetScoreBoardPlayerAIName(const TArray<FPlayerAIData>& PlayerAIDataArray)
{
	if (AIIndividualLiveScoreBoardWidget.IsValid())
	{
		AIIndividualLiveScoreBoardWidget->SetScoreBoardPlayerAIName(PlayerAIDataArray);
	}
}

void AAIIndividualGameState::SetAIIndividualWinner()
{
}
