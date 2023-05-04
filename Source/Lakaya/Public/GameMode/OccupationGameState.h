// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/LakayaBaseGameState.h"
#include "Occupation/PlayerTeam.h"
#include "OccupationGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnOccupationChangeJoinedPlayers, const uint8&)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOccupationChangeOccupationWinner, const EPlayerTeam&)
DECLARE_EVENT_TwoParams(AOccupationGameState, FTeamScoreSignature, const EPlayerTeam&, const float&)

DECLARE_EVENT_FourParams(AOccupationGameState, FKillCharacterSignature, AController*, AActor*, AController*, AActor*)

/**
 * 
 */
UCLASS()
class LAKAYA_API AOccupationGameState : public ALakayaBaseGameState
{
	GENERATED_BODY()

public:
	AOccupationGameState();

	UFUNCTION()
	void SetNumPlayers(const uint8& NewNumPlayers);

	UFUNCTION(NetMulticast, Reliable)
	void NotifyKillCharacter(AController* KilledController, AActor* KilledActor, AController* EventInstigator,
	                         AActor* Causer);

	// 현재 두 팀의 점수를 기준으로 승자를 정합니다.
	void SetOccupationWinner();

	/**
	 * @brief 팀에 점수를 부여합니다.
	 * @param Team 점수를 부여할 팀입니다.
	 * @param AdditiveScore 추가될 점수입니다.
	 */
	void AddTeamScore(const EPlayerTeam& Team, const float& AdditiveScore);

	// 해당 팀의 점수를 받아옵니다.
	float GetTeamScore(const EPlayerTeam& Team) const;

	const float& GetMaxScore() const { return MaxScore; }

	UFUNCTION()
	const EPlayerTeam& GetOccupationWinner() const { return CurrentOccupationWinner; }

	// 현재 시간을 기준으로 매칭 시간을 설정합니다. 반드시 서버에서 호출해야 합니다.
	void SetMatchTime();

	// 남은 매칭 시간을 가져옵니다. 아직 매칭시간 정보가 설정되지 않았거나, 시간이 지나간 경우 0을 반환합니다.
	float GetRemainMatchTime() const;

	// 어떤 팀이든 최대 점수에 도달한 팀이 있는지 여부를 조사합니다.
	bool IsSomeoneReachedMaxScore() const;

private:
	UFUNCTION()
	void OnRep_ATeamScore();

	UFUNCTION()
	void OnRep_BTeamScore();

	UFUNCTION()
	void OnRep_OccupationWinner();

public:
	FOnOccupationChangeOccupationWinner OnOccupationChangeOccupationWinner;
	FTeamScoreSignature OnTeamScoreChanged;
	FKillCharacterSignature OnKillCharacterNotify;

private:
	uint8 NumPlayers;

	UPROPERTY(ReplicatedUsing = OnRep_OccupationWinner, Transient)
	EPlayerTeam CurrentOccupationWinner;

	UPROPERTY(ReplicatedUsing = OnRep_ATeamScore, Transient)
	float ATeamScore;

	UPROPERTY(ReplicatedUsing = OnRep_BTeamScore, Transient)
	float BTeamScore;

	UPROPERTY(Replicated)
	float MatchStartTime;

	UPROPERTY(Replicated)
	float MatchEndingTime;

	UPROPERTY(EditAnywhere)
	float MaxScore;

	UPROPERTY(EditAnywhere)
	float MatchDuration;
};
