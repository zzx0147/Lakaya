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
				Player1Data.PlayerName = Other->GetDebugName(LocalController);
				Player2Data.PlayerName = Other->GetDebugName(LocalController);
				Player3Data.PlayerName = Other->GetDebugName(LocalController);
				Player4Data.PlayerName = Other->GetDebugName(LocalController);
				Player5Data.PlayerName = Other->GetDebugName(LocalController);
				Player6Data.PlayerName = Other->GetDebugName(LocalController);
			
				Player1Data.KillCount = Other->GetKillCount();
				Player2Data.KillCount = Other->GetKillCount();
				Player3Data.KillCount = Other->GetKillCount();
				Player4Data.KillCount = Other->GetKillCount();
				Player5Data.KillCount = Other->GetKillCount();
				Player6Data.KillCount = Other->GetKillCount();
				
				UE_LOG(LogTemp, Warning, TEXT("Set All PlayerData In AIIndividualScoreBoardWidget"));
			}
			
			FPlayerDataArray.Add(Player1Data);
			FPlayerDataArray.Add(Player2Data);
			FPlayerDataArray.Add(Player3Data);
			FPlayerDataArray.Add(Player4Data);
			FPlayerDataArray.Add(Player5Data);
			FPlayerDataArray.Add(Player6Data);
			
			SetScoreBoardPlayerName(FPlayerDataArray);
		}
	}
}

void AAIIndividualGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AIIndividualScoreBoardWidget.IsValid())
	{
		FPlayerDataArray.Empty();

		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			AController* PlayerController = It->Get();
			if (PlayerController)
			{
				ALakayaBasePlayerState* PlayerStateObj = Cast<ALakayaBasePlayerState>(PlayerController->PlayerState);
				if (PlayerStateObj)
				{
					// 점수판에 표시되는 이름 현제 컨트롤러 이름으로 표시중입니다.
					PlayerData.PlayerName = PlayerStateObj->GetDebugName(PlayerController);
					PlayerData.KillCount = PlayerStateObj->GetKillCount();
					FPlayerDataArray.Add(PlayerData);
					
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
		
		SetScoreBoardPlayerName(FPlayerDataArray);
	}
}

void AAIIndividualGameState::SetScoreBoardPlayerName(const TArray<FPlayerData>& PlayerDataArray)
{
	if (AIIndividualScoreBoardWidget.IsValid())
	{
		AIIndividualScoreBoardWidget->SetScoreBoardPlayerName(PlayerDataArray);
	}
}

void AAIIndividualGameState::SetAIIndividualWinner()
{
}
