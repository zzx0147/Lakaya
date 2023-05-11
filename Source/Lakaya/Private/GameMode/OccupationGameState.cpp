// Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/OccupationGameState.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Character/LakayaBasePlayerState.h"
#include "UI/GameLobbyCharacterSelectWidget.h"

void AOccupationGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOccupationGameState, ATeamScore);
	DOREPLIFETIME(AOccupationGameState, BTeamScore);
	DOREPLIFETIME(AOccupationGameState, CurrentOccupationWinner);
}

AOccupationGameState::AOccupationGameState()
{
	MaxScore = 100.f;
	MatchDuration = 180.f;
	ClientTeam = EPlayerTeam::None;

	PlayersByTeamMap.Add(EPlayerTeam::A, TArray<ALakayaBasePlayerState*>());
	PlayersByTeamMap.Add(EPlayerTeam::B, TArray<ALakayaBasePlayerState*>());

}

void AOccupationGameState::SetNumPlayers(const uint8& NewNumPlayers)
{
	NumPlayers = NewNumPlayers;
}

void AOccupationGameState::NotifyKillCharacter_Implementation(AController* KilledController, AActor* KilledActor,
                                                              AController* EventInstigator, AActor* Causer)
{
	OnKillCharacterNotify.Broadcast(KilledController, KilledActor, EventInstigator, Causer);
}

void AOccupationGameState::SetOccupationWinner()
{
	CurrentOccupationWinner = ATeamScore > BTeamScore ? EPlayerTeam::A : EPlayerTeam::B;
	OnOccupationChangeOccupationWinner.Broadcast(CurrentOccupationWinner);
}

void AOccupationGameState::AddTeamScore(const EPlayerTeam& Team, const float& AdditiveScore)
{
	if (Team == EPlayerTeam::A) ATeamScore += AdditiveScore;
	else if (Team == EPlayerTeam::B) BTeamScore += AdditiveScore;

	UE_LOG(LogTemp, Warning, TEXT("ATeamScore : %f"), ATeamScore);
	UE_LOG(LogTemp, Warning, TEXT("BTeamScore : %f"), BTeamScore);
}

void AOccupationGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	if (const auto BasePlayerState = Cast<ALakayaBasePlayerState>(PlayerState))
	{
		EPlayerTeam PlayerTeam = BasePlayerState->GetTeam();
		if (PlayerTeam == EPlayerTeam::A || PlayerTeam == EPlayerTeam::B)//팀이 있으면 바로 배열에 추가
		{
			PlayersByTeamMap[PlayerTeam].Add(BasePlayerState);
			if (ClientTeam == PlayerTeam)
			{
				CharacterSelectWidget->RegisterPlayer(BasePlayerState);
			}
		}
		else//없으면 팀이 변경되었을때 추가
		{
			BasePlayerState->OnTeamChanged.AddLambda([this, BasePlayerState](const EPlayerTeam& ArgTeam)
				{
					if (ArgTeam == EPlayerTeam::A)
					{
						GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("TeamChange! A!"));
					}
					else if (ArgTeam == EPlayerTeam::B)
					{
						GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("TeamChange! B!"));
					}
					else if (ArgTeam == EPlayerTeam::None)
					{
						GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("TeamChange! NONE!"));
					}
					else if (ArgTeam == EPlayerTeam::Individual)
					{
						GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("TeamChange! Individual!"));
					}

					if (ArgTeam == EPlayerTeam::A || ArgTeam == EPlayerTeam::B)
					{
						PlayersByTeamMap[ArgTeam].Add(BasePlayerState);
						if (ClientTeam == ArgTeam)//클라이언트 팀이 NONE 일 수 있음
						{
							CharacterSelectWidget->RegisterPlayer(BasePlayerState);
						}
					}
				});
		}
	}
}


float AOccupationGameState::GetTeamScore(const EPlayerTeam& Team) const
{
	if (Team == EPlayerTeam::A) return ATeamScore;
	if (Team == EPlayerTeam::B) return BTeamScore;
	UE_LOG(LogScript, Warning, TEXT("Trying to GetTeamScore with not valid value! it was %d"), Team);
	return 0.f;
}

bool AOccupationGameState::IsSomeoneReachedMaxScore() const
{
	return ATeamScore >= MaxScore || BTeamScore >= MaxScore;
}

void AOccupationGameState::OnRep_ATeamScore()
{
	OnTeamScoreChanged.Broadcast(EPlayerTeam::A, ATeamScore);
}

void AOccupationGameState::OnRep_BTeamScore()
{
	OnTeamScoreChanged.Broadcast(EPlayerTeam::B, ATeamScore);
}

void AOccupationGameState::OnRep_OccupationWinner()
{
	OnOccupationChangeOccupationWinner.Broadcast(CurrentOccupationWinner);
}

void AOccupationGameState::CreateCharacterSelectWidget(APlayerController* LocalController)
{
	Super::CreateCharacterSelectWidget(LocalController);

	if (const auto BasePlayerState = LocalController->GetPlayerState<ALakayaBasePlayerState>())
	{
		const auto NewTeam = BasePlayerState->GetTeam();
		if (NewTeam == EPlayerTeam::A || NewTeam == EPlayerTeam::B)
		{
			ClientTeam = NewTeam;//로컬 컨트롤러의 팀을 저장한다.(로컬 컨트롤러의 팀에 따라 표기할 팀이 달라짐)
			
			for (const auto temp : PlayersByTeamMap[ClientTeam])
			{
				CharacterSelectWidget->RegisterPlayer(temp);
				//현재까지 등록된 플레이어 스테이트들을 위젯에 등록한다
			}
		}

		else
		{
			BasePlayerState->OnTeamChanged.AddLambda([this, BasePlayerState](const EPlayerTeam& ArgTeam)
				{
					if (ArgTeam == EPlayerTeam::A || ArgTeam == EPlayerTeam::B)
					{
						ClientTeam = ArgTeam;
						for (const auto temp : PlayersByTeamMap[ClientTeam])
						{
							CharacterSelectWidget->RegisterPlayer(temp);
							//현재까지 등록된 플레이어 스테이트들을 위젯에 등록한다
						}
					}
				});
		}	
	}
}
