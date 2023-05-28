// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/AIIndividualGameState.h"

#include "Character/LakayaBasePlayerState.h"
#include "Kismet/GameplayStatics.h"

AAIIndividualGameState::AAIIndividualGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FClassFinder<UAIIndividualScoreBoardWidget> AIIndividualScoreBoardFinder(
	TEXT("/Game/Blueprints/UMG/IndividualWidget/WBP_AIIndividualScoreBoardWidget"));

	if (AIIndividualScoreBoardFinder.Succeeded()) AIIndividualScoreBoardWidgettClass = AIIndividualScoreBoardFinder.Class;
}

void AAIIndividualGameState::BeginPlay()
{
	Super::BeginPlay();

	if (const auto LocalController = GetWorld()->GetFirstPlayerController<APlayerController>())
	{
		AIIndividualScoreBoardWidget = CreateWidget<UAIIndividualScoreBoardWidget>(LocalController, AIIndividualScoreBoardWidgettClass);
		
		if (AIIndividualScoreBoardWidget.IsValid())
		{
			AIIndividualScoreBoardWidget->AddToViewport();
			AIIndividualScoreBoardWidget->SetVisibility(ESlateVisibility::Visible);
			
			if (const auto Other = LocalController->GetPlayerState<ALakayaBasePlayerState>())
			{
				PlayerAI1Data.PlayerName = Other->GetDebugName(LocalController);
				PlayerAI2Data.PlayerName = Other->GetDebugName(LocalController);
				PlayerAI3Data.PlayerName = Other->GetDebugName(LocalController);
				PlayerAI4Data.PlayerName = Other->GetDebugName(LocalController);
				PlayerAI5Data.PlayerName = Other->GetDebugName(LocalController);
				PlayerAI6Data.PlayerName = Other->GetDebugName(LocalController);
			          
				PlayerAI1Data.KillCount = Other->GetKillCount();
				PlayerAI2Data.KillCount = Other->GetKillCount();
				PlayerAI3Data.KillCount = Other->GetKillCount();
				PlayerAI4Data.KillCount = Other->GetKillCount();
				PlayerAI5Data.KillCount = Other->GetKillCount();
				PlayerAI6Data.KillCount = Other->GetKillCount();
				
				UE_LOG(LogTemp, Warning, TEXT("Set All PlayerData In AIIndividualScoreBoardWidget"));
			}
			
			FPlayerAIDataArray.Add(PlayerAI1Data);
			FPlayerAIDataArray.Add(PlayerAI2Data);
			FPlayerAIDataArray.Add(PlayerAI3Data);
			FPlayerAIDataArray.Add(PlayerAI4Data);
			FPlayerAIDataArray.Add(PlayerAI5Data);
			FPlayerAIDataArray.Add(PlayerAI6Data);
			
			SetScoreBoardPlayerName(FPlayerAIDataArray);
		}
	}
}

void AAIIndividualGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AIIndividualScoreBoardWidget.IsValid())
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
					
					// PlayerStateObj->OnKillCountChanged.AddLambda([this, PlayerStateObj]
					// 	(const uint16& NewKillCount)
					// {
					// 	PlayerData.PlayerName = PlayerStateObj->GetCharacterName();
					// 	PlayerData.KillCount = NewKillCount;
					// 	FPlayerDataArray.Add(PlayerData);
					//
					// 	UE_LOG(LogTemp, Warning, TEXT("킬 적립!"));
					// });
				}
			}
		}
		
		SetScoreBoardPlayerName(FPlayerAIDataArray);
	}
}

void AAIIndividualGameState::SetScoreBoardPlayerName(const TArray<FPlayerAIData>& PlayerDataArray)
{
	if (AIIndividualScoreBoardWidget.IsValid())
	{
		AIIndividualScoreBoardWidget->SetScoreBoardPlayerName(PlayerDataArray);
	}
}

void AAIIndividualGameState::SetAIIndividualWinner()
{
}
