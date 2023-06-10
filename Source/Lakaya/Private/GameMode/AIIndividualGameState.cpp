// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/AIIndividualGameState.h"

#include "AI/AiCharacterController.h"
#include "UI/IndividualWidget/IndividualLiveScoreBoardWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerController/InteractablePlayerController.h"
#include "UI/MatchStartWaitWidget.h"

AAIIndividualGameState::AAIIndividualGameState()
{
	PrimaryActorTick.bCanEverTick = true;

	MatchStartWaitWidgetLifeTime = 3.0f;
	
	static ConstructorHelpers::FClassFinder<UIndividualLiveScoreBoardWidget> AIIndividualLiveScoreBoardFinder(
	TEXT("/Game/Blueprints/UMG/IndividualWidget/WBP_IndividualLiveScoreBoardWidget"));

	if (AIIndividualLiveScoreBoardFinder.Succeeded()) AIIndividualLiveScoreBoardWidgetClass = AIIndividualLiveScoreBoardFinder.Class;
}

void AAIIndividualGameState::BeginPlay()
{
	Super::BeginPlay();
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
			
			if(PlayerController && PlayerController->IsA<AInteractablePlayerController>())
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
					// 점수판에 표시되는 이름 현재 컨트롤러 이름으로 표시중입니다.
					PlayerAIData.PlayerName = PlayerStateObj->GetDebugName(PlayerController);
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

	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
		AIIndividualLiveScoreBoardWidget = CreateWidget<UIndividualLiveScoreBoardWidget>(LocalController, AIIndividualLiveScoreBoardWidgetClass);
	
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AController* AllControllers = It->Get();
		APlayerController* PlayerCharacterController = Cast<APlayerController>(AllControllers);

		if (PlayerCharacterController)
		{
			PlayerCharacterController->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_None);
		}

		if (AllControllers && AIIndividualLiveScoreBoardWidget.IsValid())
		{
			AIIndividualLiveScoreBoardWidget->AddToViewport();
			AIIndividualLiveScoreBoardWidget->SetVisibility(ESlateVisibility::Visible);
			
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

			if (PlayerCharacterController)
			{
				PlayerCharacterController->GetCharacter()->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}
			
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
