#include "GameMode/AIIndividualGameState.h"

#include "IndividualTabMinimapWidget.h"
#include "AI/AiCharacterController.h"
#include "AI/AiDroneController.h"
#include "Character/LakayaBasePlayerState.h"
#include "ETC/OutlineManager.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "UI/AIIndividualFinalResultWidget.h"
#include "UI/AIIndividualLiveScoreBoardWidget.h"
#include "UI/GameLobbyCharacterSelectWidget.h"
#include "UI/IndividualOverlayMinimapWidget.h"
#include "UI/IndividualWidget/IndividualGameResultWidget.h"
#include "UI/IndividualWidget/IndividualLiveScoreBoardWidget.h"
#include "UI/GameScoreBoardWidget.h"

AAIIndividualGameState::AAIIndividualGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	MatchStartWaitWidgetLifeTime = 3.0f;
	AIIndividualFinalResultLifeTime = 4.0f;

	ClientTeam = ETeam::Individual;
	IndividualLiveScoreBoardWidgetClass = UIndividualLiveScoreBoardWidget::StaticClass();
	AIIndividualLiveScoreBoardWidgetClass = UAIIndividualLiveScoreBoardWidget::StaticClass();
	HUDMinimapWidgetClass = UIndividualOverlayMinimapWidget::StaticClass();
	TabMinimapWidgetClass = UIndividualTabMinimapWidget::StaticClass();
}

void AAIIndividualGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	if (AIIndividualLiveScoreBoardWidget.IsValid()) AIIndividualLiveScoreBoardWidget->RegisterPlayer(PlayerState);

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

void AAIIndividualGameState::SetScoreBoardVisibility(const bool& Visible)
{
	Super::SetScoreBoardVisibility(Visible);

	if (MatchState == MatchState::InProgress)
	{
		InternalSetScoreBoardVisibility(Visible);
	}
}

void AAIIndividualGameState::SetTabMinimapVisibility(const bool& Visible)
{
	Super::SetTabMinimapVisibility(Visible);

	if (MatchState == MatchState::InProgress)
	{
		InternalSetTabMinimapVisibility(Visible);
	}
}

void AAIIndividualGameState::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnOutlineManager()) OutlineManager->SetTeam(ETeam::Individual);

	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		IndividualLiveScoreBoardWidget = CreateWidget<UIndividualLiveScoreBoardWidget>(
			LocalController, IndividualLiveScoreBoardWidgetClass);
		
		if (AIIndividualLiveScoreBoardWidgetClass)
		{
			AIIndividualLiveScoreBoardWidget = CreateWidget<UAIIndividualLiveScoreBoardWidget>(LocalController, AIIndividualLiveScoreBoardWidgetClass);
			if (AIIndividualLiveScoreBoardWidget.IsValid())
			{
				AIIndividualLiveScoreBoardWidget->AddToViewport();
				AIIndividualLiveScoreBoardWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
				for (auto& Player : PlayerArray) AIIndividualLiveScoreBoardWidget->RegisterPlayer(Player);
			}
		}

		if (GameResultWidgetClass)
		{
			GameResultWidget = CreateWidget<UIndividualGameResultWidget>(LocalController, GameResultWidgetClass);
			if (GameResultWidget == nullptr) UE_LOG(LogTemp, Warning, TEXT("GameResultWidget is null."));
			GameResultWidget->AddToViewport(1);
			GameResultWidget->SetVisibility(ESlateVisibility::Hidden);
		}
		else UE_LOG(LogTemp, Warning, TEXT("GameResultWidgeTClass is null"));

		if (HUDMinimapWidgetClass)
		{
			HUDMinimapWidget = CreateWidget<UIndividualOverlayMinimapWidget>(
				LocalController, HUDMinimapWidgetClass);
			if (HUDMinimapWidget)
			{
				HUDMinimapWidget->AddToViewport();
				HUDMinimapWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (AIIndividualFinalResultWidgetClass)
		{
			AIIndividualFinalResultWidget = CreateWidget<UAIIndividualFinalResultWidget>(
				LocalController, AIIndividualFinalResultWidgetClass);
			if (AIIndividualFinalResultWidget)
			{
				AIIndividualFinalResultWidget->AddToViewport(30);
				AIIndividualFinalResultWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (TabMinimapWidgetClass)
		{
			TabMinimapWidget = CreateWidget<UIndividualTabMinimapWidget>(
				LocalController, TabMinimapWidgetClass);
			if (TabMinimapWidget)
			{
				TabMinimapWidget->AddToViewport();
				TabMinimapWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AController* AllControllers = It->Get();

		if (Cast<AAiDroneController>(AllControllers)) continue;

		const auto IndividualPlayerState = Cast<ALakayaBasePlayerState>(
			AllControllers->GetPlayerState<ALakayaBasePlayerState>());

		// AI와 플레이어들의 정보를 Array에 담습니다.
		PlayerArrays.Add(IndividualPlayerState);

		if (AllControllers && IndividualLiveScoreBoardWidget.IsValid())
		{
			IndividualLiveScoreBoardWidget->AddToViewport();
			IndividualLiveScoreBoardWidget->SetVisibility(ESlateVisibility::Visible);

			ALakayaBasePlayerState* PlayerStateObj = Cast<ALakayaBasePlayerState>(AllControllers->PlayerState);

			// 매치시작전에 스폰된 플레이어와 AI의 정보를 미리 한번 넣어주는곳입니다. 이후 Tick에서 실시간으로 계속 검사합니다.
			if (AllControllers && AllControllers->IsPlayerController())
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
			if (AllControllers && !AllControllers->IsPlayerController())
			{
				if (PlayerStateObj)
				{
					// 점수판에 표시되는 이름이며 현재 AI 의 캐릭터 + 몃번째의 AI 컨트롤러 번호인지를 표시해주고있습니다.
					AIName = PlayerStateObj->GetCharacterName().ToString()
						+ " AI (" + FString::FromInt(static_cast<int>(It.GetIndex())) + ")";

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
	if (IndividualLiveScoreBoardWidget.IsValid())
	{
		FPlayerAIDataArray.Empty();

		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			AController* AllControllers = It->Get();
			if (Cast<AAiDroneController>(AllControllers)) continue;
			ALakayaBasePlayerState* PlayerStateObj = Cast<ALakayaBasePlayerState>(AllControllers->PlayerState);

			if (AllControllers && AllControllers->IsPlayerController())
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
			if (AllControllers && !AllControllers->IsPlayerController())
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

	if (HUDMinimapWidget)
	{
		HUDMinimapWidget->SetUpdateMinimap(true);
		HUDMinimapWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		HUDMinimapWidget->SetEnemyTeam(ETeam::Individual);
		HUDMinimapWidget->SetTeam(ETeam::Individual);
		
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			const AController* AllControllers = It->Get();

			const auto IndividualPlayerState = Cast<ALakayaBasePlayerState>(
				AllControllers->GetPlayerState<ALakayaBasePlayerState>());

			HUDMinimapWidget->SetIndividualPlayersByMinimap(IndividualPlayerState, HUDMinimapWidget->CreatePlayerImage(ETeam::Individual));
		}
	}

	if (TabMinimapWidget)
	{
		TabMinimapWidget->SetUpdateMinimap(true);
		TabMinimapWidget->SetEnemyTeam(ETeam::Individual);
		TabMinimapWidget->SetTeam(ETeam::Individual);
		// TabMinimapWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			const AController* AllControllers = It->Get();

			const auto IndividualPlayerState = Cast<ALakayaBasePlayerState>(
				AllControllers->GetPlayerState<ALakayaBasePlayerState>());

			TabMinimapWidget->SetIndividualPlayersByMinimap(IndividualPlayerState, TabMinimapWidget->CreatePlayerImage(ETeam::Individual));
		}
	}
	
	// 매치 시작하면 플레이어 인풋 막기
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		// LocalController->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_None);

		LocalController->DisableInput(LocalController);
	}

	FTimerDelegate TimerDelegate;
	GetWorldTimerManager().SetTimer(TimerHandle_WaitTimerHandle, TimerDelegate, MatchStartWaitWidgetLifeTime, false);

	// 게임이 본격적으로 시작이 되면 AI 의 비헤이비어 트리를 시작시켜줍니다.
	TimerDelegate.BindWeakLambda(this,[this]
	{
		if (this == nullptr) return;

		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			AController* AllControllers = It->Get();

			APlayerController* PlayerCharacterController = Cast<APlayerController>(AllControllers);
			AAiCharacterController* AiCharacterController = Cast<AAiCharacterController>(AllControllers);

			// if (AllControllers && AIIndividualLiveScoreBoardWidget.IsValid())
			// 	AIIndividualLiveScoreBoardWidget->SetVisibility(ESlateVisibility::Visible);

			// 플레이어 인풋 동작 
			if (PlayerCharacterController)
			{
				// PlayerCharacterController->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

				PlayerCharacterController->EnableInput(PlayerCharacterController);
			}

			// AI 동작
			if (AiCharacterController)
			{
				AiCharacterController->BlackboardComp->InitializeBlackboard(
					*(AiCharacterController->BehaviorTreeAsset->BlackboardAsset));
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

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AController* AllControllers = It->Get();
		
		if(AllControllers)
		{
			AAiCharacterController* AiCharacterController = Cast<AAiCharacterController>(AllControllers);
			AAiDroneController* AiDroneController = Cast<AAiDroneController>(AllControllers);
		
			// AI 동작 정지
			if (AiCharacterController) AiCharacterController->BehaviorTreeComp->StopTree();
			if (AiDroneController) AiDroneController->BehaviorTreeComp->StopTree();
		}
	}

	if (GameResultWidget.IsValid())
		GameResultWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// 플레이어의 정보를 가져와 게임결과 위젯을 바인딩해줍니다.
	if (const auto LocalPlayerController = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		const auto IndividualPlayerState = Cast<ALakayaBasePlayerState>(
			LocalPlayerController->GetPlayerState<ALakayaBasePlayerState>());
		if (IndividualPlayerState == nullptr) UE_LOG(LogTemp, Warning, TEXT("IndividualPlayerState is null."));

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

	
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>();
		LocalController && LocalController->IsLocalController())
	{
		PlayerArrays.Sort([](const TWeakObjectPtr<ALakayaBasePlayerState,FWeakObjectPtr>& A,
			const TWeakObjectPtr<ALakayaBasePlayerState,FWeakObjectPtr>& B)
		{
			const ALakayaBasePlayerState* PlayerStateA = A.Get();
			const ALakayaBasePlayerState* PlayerStateB = B.Get();

			if (!PlayerStateA || !PlayerStateB) return false;

			return PlayerStateA->GetKillCount() > PlayerStateB->GetKillCount();
		});
		
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindWeakLambda(this, [&]
		{
			GameResultWidget->SetVisibility(ESlateVisibility::Hidden);
			AIIndividualFinalResultWidget->SetMatchResultData(PlayerAIData.KillCount, PlayerArrays);
			AIIndividualFinalResultWidget->SetVisibility(ESlateVisibility::Visible);
		});
		GetWorldTimerManager().SetTimer(TimerHandle_AIIndividualFinalResultHandle, TimerDelegate, AIIndividualFinalResultLifeTime, false);
	}
	
	// FTimerManager Timers;
	// Timers.ClearAllTimersForObject(GetWorld());
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

void AAIIndividualGameState::InternalSetScoreBoardVisibility(const bool& Visible) const
{
	if (!ScoreBoard.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("AIIndividualGameState_ScoreBoard is null."));
		return;
	}

	ScoreBoard->SetVisibility(Visible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void AAIIndividualGameState::InternalSetTabMinimapVisibility(const bool& Visible) const
{
	if (!TabMinimapWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIIndividualGameState_AIIndividualTabMinimapWidget is null."));
		return;
	}

	TabMinimapWidget->SetVisibility(Visible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
}

void AAIIndividualGameState::UpdatePlayerByMinimap(const ALakayaBasePlayerState* NewPlayerState,
	const UImage* NewPlayerImage)
{
	if (!HUDMinimapWidget || !TabMinimapWidget) return;

	for (const auto& Player : PlayerArrays)
	{
		
	}
}

void AAIIndividualGameState::SetClientTeam(const ETeam& NewTeam)
{
	Super::SetClientTeam(NewTeam);

	ClientTeam = NewTeam;
}

void AAIIndividualGameState::SetScoreBoardPlayerAIName(const TArray<FPlayerAIData>& PlayerAIDataArray) const
{
	if (IndividualLiveScoreBoardWidget.IsValid())
	{
		IndividualLiveScoreBoardWidget->SetScoreBoardPlayerAIName(PlayerAIDataArray);
	}
}