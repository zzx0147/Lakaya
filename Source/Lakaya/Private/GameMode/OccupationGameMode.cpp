// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/OccupationGameMode.h"

#include "Character/LakayaBaseCharacter.h"
#include "GameMode/OccupationGameState.h"
#include "PlayerController/MovablePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Occupation/CaptureArea.h"

AOccupationGameMode::AOccupationGameMode()
{
	DefaultPawnClass = ALakayaBaseCharacter::StaticClass();
	PlayerControllerClass = AMovablePlayerController::StaticClass();
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
	TArray<AActor*> CaptureAreas;
	
	UGameplayStatics::GetAllActorsOfClass(this,ACaptureArea::StaticClass(),CaptureAreas);
	
	for(const auto CaptureArea : CaptureAreas)
	{
		GetWorldTimerManager().ClearAllTimersForObject(CaptureArea);
	}
}

void AOccupationGameMode::HandleMatchIsSelectCharacter()
{
	Super::HandleMatchIsSelectCharacter();

	// TODO : 최대 인원보다 더 많은 인원이 방에 입장했을 시에는, 일단 모든 팀원들을 팀배정을 해줍니다.
	if (OccupationGameState->GetMaximumPlayers() == OccupationGameState->PlayerArray.Num())
	{
		AssignTeams(OccupationGameState->GetMaximumPlayers());
	}
	else 
	{
		AssignTeams(OccupationGameState->PlayerArray.Num());
	}
}

void AOccupationGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	if(!OccupationGameState) return;
	
	if (GetNumPlayers() >= OccupationGameState->GetMaximumPlayers())
	{
		ErrorMessage = TEXT("The server is full.");
	}
	
}

void AOccupationGameMode::AssignTeams(const uint8 PlayerCount) const
{
	for (int i = 0; i < PlayerCount; i++)
	{
		auto* LakayaBasePlayerState = Cast<ALakayaBasePlayerState>(OccupationGameState->PlayerArray[i]);
		if (LakayaBasePlayerState == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("OccupationGameMode_LakayaBasePlayerState is null."));	
		}

		auto Team = i % 2 == 0 ? ETeam::Anti : ETeam::Pro;
		LakayaBasePlayerState->SetTeam(Team);

		UE_LOG(LogTemp, Warning, TEXT("%s"), Team == ETeam::Anti ? TEXT("Anti팀에 배정되었습니다.") : TEXT("Pro팀에 배정되었습니다."));
	}
}