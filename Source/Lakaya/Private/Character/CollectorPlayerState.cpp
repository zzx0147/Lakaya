// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/CollectorPlayerState.h"

#include "InterchangeResult.h"
#include "OccupationCharacter.h"
#include "GameMode/OccupationGameMode.h"
#include "GameMode/OccupationGameState.h"
#include "Net/UnrealNetwork.h"

ACollectorPlayerState::ACollectorPlayerState()
{
	// TODO: Steam이름을 잘 가져오는지 테스트 해봐야 함.
	PlayerInfo.PlayerName = GetName();
	PlayerInfo.TotalScore = 0;
	PlayerInfo.Kills = 0;
	PlayerInfo.OccupationSuccess = 0;
	PlayerInfo.OwnObjectNum = 0;
}

void ACollectorPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACollectorPlayerState, PlayerTeamState);
	DOREPLIFETIME(ACollectorPlayerState, PlayerInfo);
}

void ACollectorPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	OnPawnSet.AddDynamic(this, &ACollectorPlayerState::OnPawnSetCallback);
}

void ACollectorPlayerState::SetPlayerTeamState(EPlayerTeamState TeamState)
{
	const AOccupationGameMode* GameMode = Cast<AOccupationGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("CollectorPlayerState_GameMode is null."));
		return;
	}

	if (GameMode->GetMatchState() == MatchState::WaitingToStart)
	{
		if (PlayerTeamState == EPlayerTeamState::None)
		{
			PlayerTeamState = TeamState;
			if (const auto CastedPawn = Cast<AOccupationCharacter>(GetPawn()))
				CastedPawn->SetTeam(PlayerTeamState);
		}
		else UE_LOG(LogTemp, Warning, TEXT("이미 팀이 배정 되었습니다."));
	}
}

void ACollectorPlayerState::AddTotalScore()
{
	// TODO : 본인이 점령한 오브젝트가 있을 경우에만 실행이 되도록.
	// 초마다 올라가는 점수.
	PlayerInfo.TotalScore += (50 * PlayerInfo.OccupationSuccess);
}

void ACollectorPlayerState::AddKills()
{
	PlayerInfo.Kills++;
	PlayerInfo.TotalScore += 100;
	// OnRep_ScoreUpdate(PlayerInfo);
}

void ACollectorPlayerState::AddOccupationSuccess()
{
	PlayerInfo.OccupationSuccess++;
	PlayerInfo.TotalScore += 500;
}

void ACollectorPlayerState::SetOwnObjectNum(int32 Num)
{
	PlayerInfo.OwnObjectNum += Num;
	if (PlayerInfo.OwnObjectNum < 0)
		PlayerInfo.OwnObjectNum = 0;
}

void ACollectorPlayerState::OnPawnSetCallback(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (const auto CastedPawn = Cast<AOccupationCharacter>(NewPawn))
		CastedPawn->SetTeam(PlayerTeamState);
}

void ACollectorPlayerState::OnRep_BroadCastMyTeam()
{
	if (const auto CastedPawn = Cast<AOccupationCharacter>(GetPawn()))
		CastedPawn->SetTeam(PlayerTeamState);
}

void ACollectorPlayerState::OnRep_ScoreUpdate()
{
	UE_LOG(LogTemp, Warning, TEXT("플레이어 이름 : %s 점수 : %d 킬 : %d 점령 성공 : %d"), *PlayerInfo.PlayerName, PlayerInfo.TotalScore, PlayerInfo.Kills, PlayerInfo.OccupationSuccess);
	OnPlayerStateChange.Broadcast(PlayerInfo, PlayerInfo.TotalScore, PlayerInfo.Kills, PlayerInfo.OccupationSuccess, PlayerInfo.OwnObjectNum);
	UE_LOG(LogTemp, Warning, TEXT("OnRep_ScoreUpdate Function."));
}

void ACollectorPlayerState::OnRep_OwnObjectNum()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_OwnObjectNum Function."));
}