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
			if (PlayerStateObj)
			{
				PlayerAIData.PlayerName = PlayerStateObj->GetDebugName(AllControllers);
				PlayerAIData.KillCount = PlayerStateObj->GetKillCount();
				FPlayerAIDataArray.Add(PlayerAIData);
				
				UE_LOG(LogTemp, Warning, TEXT("Set All PlayerData In AIIndividualLiveScoreBoardWidget"));
			}
			
			SetScoreBoardPlayerAIName(FPlayerAIDataArray);
		}
	}
}

void AAIIndividualGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AIIndividualLiveScoreBoardWidget.IsValid())
	{
		FPlayerAIDataArray.Empty();

		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			AController* PlayerController = It->Get();
			ALakayaBasePlayerState* PlayerStateObj = Cast<ALakayaBasePlayerState>(PlayerController->PlayerState);

			if (PlayerController && PlayerController->IsA<AInteractablePlayerController>())
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
			if (PlayerController && !PlayerController->IsA<AInteractablePlayerController>())
			{
				if (PlayerStateObj)
				{
					// 점수판에 표시되는 이름이며 현재 AI 의 캐릭터 + 몃번째의 AI 컨트롤러 번호인지를 표시해주고있습니다.
					FString AIName = PlayerStateObj->GetCharacterName().ToString()
					+ " AI (" +  FString::FromInt(static_cast<int>(It.GetIndex())) + ")";
					PlayerAIData.PlayerName = AIName;
					
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

	// 매치 시작하면 플레이어 동작 막기
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
		LocalController->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_None);
	
	FTimerDelegate TimerDelegate;
	GetWorldTimerManager().SetTimer(TimerHandle_WaitTimerHandle, TimerDelegate, MatchStartWaitWidgetLifeTime, false);

	// 게임이 본격적으로 시작이 되면 AI 의 비헤이비어 트리를 시작시켜줍니다.
	TimerDelegate.BindLambda([this]
	{
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			AController* AllControllers = It->Get();

			APlayerController* PlayerCharacterController = Cast<APlayerController>(AllControllers);
			AAiCharacterController* AiCharacterController = Cast<AAiCharacterController>(AllControllers);

			if (AllControllers && AIIndividualLiveScoreBoardWidget.IsValid())
				AIIndividualLiveScoreBoardWidget->SetVisibility(ESlateVisibility::Visible);

			// 플레이어 동작 
			if (PlayerCharacterController)
				PlayerCharacterController->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

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

	// 플레이어들의 정보가 담긴 배열을 내림차순으로 정렬합니다.
	// std::sort(AllPlayersArray.begin(), AllPlayersArray.end(), []
	// 	(const TWeakObjectPtr<ALakayaBasePlayerState>& A,
	// 	const TWeakObjectPtr<ALakayaBasePlayerState>& B)
	// {
	// 	return A->GetKillCount() > B->GetKillCount();
	// });

	for (int i = 0; i < AllPlayersArray.Num(); ++i)
	{
		AllPlayersArray.Sort([](const TWeakObjectPtr<ALakayaBasePlayerState>& A,
		                        const TWeakObjectPtr<ALakayaBasePlayerState>& B)
		{
			return A->GetTotalScore() > B->GetTotalScore();
		});
	}

	// 현재 플레이어 컨트롤러 반환
	const auto CurrentPlayerState = GetWorld()->GetFirstPlayerController()->GetPlayerState<ALakayaBasePlayerState>();
	uint8 CurrentPlayerIndex = INDEX_NONE;
	for (uint8 i = 0; i < AllPlayersArray.Num(); ++i)
	{
		if (AllPlayersArray[i].Get() == CurrentPlayerState)
		{
			CurrentPlayerIndex = i;
			break;
		}
	}

	if (CurrentPlayerIndex != INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("플레이어의 순위 : %d"), CurrentPlayerIndex);
		switch (CurrentPlayerIndex)
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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("현재 플레이어를 찾을 수 없습니다."));
	}
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
