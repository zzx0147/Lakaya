// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/OccupationGameMode.h"
#include "Character/InteractableCharacter.h"
#include "Character/StatPlayerState.h"
#include "GameMode/OccupationGameState.h"
#include "PlayerController/InteractablePlayerController.h"

AOccupationGameMode::AOccupationGameMode() : Super()
{
	DefaultPawnClass = AInteractableCharacter::StaticClass();
	PlayerControllerClass = AInteractablePlayerController::StaticClass();
	PlayerStateClass = AStatPlayerState::StaticClass();
	GameStateClass = AOccupationGameState::StaticClass();

	MatchStartDelay = 3.0f;
}

void AOccupationGameMode::OnPlayerKilled(AController* VictimController, AController* InstigatorController, AActor* DamageCauser)
{
	Super::OnPlayerKilled(VictimController, InstigatorController, DamageCauser);
}

void AOccupationGameMode::BeginPlay()
{
	Super::BeginPlay();
	OccupationGameState = GetGameState<AOccupationGameState>();
	UE_LOG(LogTemp, Warning, TEXT("Occupation Game Mode Begin Play"));
}

void AOccupationGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();

	OccupationGameState->SetOccupationWinner();
}

void AOccupationGameMode::RegisterPlayer(AController* NewPlayer)
{
	Super::RegisterPlayer(NewPlayer);

	if (CurrentPlayerNum == BaseGameState->GetMaximumPlayers())
	{
		GetWorldTimerManager().SetTimer(TimerHandle_DelayedMatchStart, this, &AOccupationGameMode::HandleMatchHasStarted,
			MatchStartDelay, false);
	}
}

void AOccupationGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	if (OccupationGameState->GetMaximumPlayers() == OccupationGameState->PlayerArray.Num())
	{
		for (int i = 0; i < OccupationGameState->GetMaximumPlayers(); i++)
		{
			auto* LakayaBasePlayerState = Cast<ALakayaBasePlayerState>(OccupationGameState->PlayerArray[i]);
			if (LakayaBasePlayerState == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_LakayaBasePlayerState is null."));	
			}

			const auto Team = i % 2 == 0 ? ETeam::Anti : ETeam::Pro;
			LakayaBasePlayerState->SetTeam(Team);
			UE_LOG(LogTemp, Warning, TEXT("%s"), Team == ETeam::Anti ? TEXT("Anti팀에 배정되었습니다.") : TEXT("Pro팀에 배정되었습니다."));
		}
	}
}

// TODO : MatchStart에서 팀 배정을 해줘야 함.
// void AOccupationGameMode::HandleMatchIsSelectCharacter()
// {
// 	Super::HandleMatchIsSelectCharacter();
// 	
// 	if (OccupationGameState->GetMaximumPlayers() == OccupationGameState->PlayerArray.Num())
// 	{
// 		for (int i = 0; i < OccupationGameState->GetMaximumPlayers(); i++)
// 		{
// 			if (OccupationGameState->PlayerArray.IsValidIndex(i))
// 			{
// 				auto* LakayaBasePlayerState = Cast<ALakayaBasePlayerState>(OccupationGameState->PlayerArray[i]);
// 				if (LakayaBasePlayerState == nullptr)
// 				{
// 					UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_CollectorPlayerState is null."));
// 				}
//
// 				const auto Team = i % 2 == 0 ? ETeam::Anti : ETeam::Pro;
// 				LakayaBasePlayerState->SetTeam(Team);
// 				UE_LOG(LogTemp, Warning, TEXT("%s"), i % 2 == 0 ? TEXT("A팀에 배정 되었습니다.") : TEXT("B팀에 배정 되었습니다."));
// 			}
// 		}
// 	}
// }