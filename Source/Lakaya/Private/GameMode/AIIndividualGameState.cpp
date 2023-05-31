// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/AIIndividualGameState.h"

#include "UI/IndividualWidget/IndividualLiveScoreBoardWidget.h"
#include "Character/LakayaBasePlayerState.h"
#include "Kismet/GameplayStatics.h"

AAIIndividualGameState::AAIIndividualGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FClassFinder<UIndividualLiveScoreBoardWidget> AIIndividualLiveScoreBoardFinder(
	TEXT("/Game/Blueprints/UMG/IndividualWidget/WBP_IndividualLiveScoreBoardWidget"));

	if (AIIndividualLiveScoreBoardFinder.Succeeded()) AIIndividualLiveScoreBoardWidgetClass = AIIndividualLiveScoreBoardFinder.Class;
}

void AAIIndividualGameState::BeginPlay()
{
	Super::BeginPlay();
	
	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
		AIIndividualLiveScoreBoardWidget = CreateWidget<UIndividualLiveScoreBoardWidget>(LocalController, AIIndividualLiveScoreBoardWidgetClass);

	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AController* PlayerController = It->Get();

		if (PlayerController && AIIndividualLiveScoreBoardWidget.IsValid())
		{
			AIIndividualLiveScoreBoardWidget->AddToViewport();
			AIIndividualLiveScoreBoardWidget->SetVisibility(ESlateVisibility::Visible);
			
			ALakayaBasePlayerState* PlayerStateObj = Cast<ALakayaBasePlayerState>(PlayerController->PlayerState);
			if (PlayerStateObj)
			{
				PlayerAIData.PlayerName = PlayerStateObj->GetDebugName(PlayerController);
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
			if (PlayerController)
			{
				ALakayaBasePlayerState* PlayerStateObj = Cast<ALakayaBasePlayerState>(PlayerController->PlayerState);
				if (PlayerStateObj)
				{
					// 점수판에 표시되는 이름 현제 컨트롤러 이름으로 표시중입니다.
					PlayerAIData.PlayerName = PlayerStateObj->GetDebugName(PlayerController);
					PlayerAIData.KillCount = PlayerStateObj->GetKillCount();
					FPlayerAIDataArray.Add(PlayerAIData);
				}
			}
		}
		
		SetScoreBoardPlayerAIName(FPlayerAIDataArray);
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
