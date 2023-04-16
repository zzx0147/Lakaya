// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Occupation/PlayerTeamState.h"
#include "OccupationGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnOccupationChangeJoinedPlayers, const uint8&)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOccupationChangeOccupationWinner, const EPlayerTeamState&)
DECLARE_EVENT_TwoParams(AOccupationGameState, FTeamScoreSignature, const EPlayerTeamState&, const float&)
DECLARE_EVENT_FourParams(AOccupationGameState, FKillCharacterSignature, AController*, AActor*, AController*, AActor*)

/**
 * 
 */
UCLASS()
class LAKAYA_API AOccupationGameState : public AGameState
{
	GENERATED_BODY()

public:
	AOccupationGameState();

protected:
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;

public:
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
	void AddTeamScore(const EPlayerTeamState& Team, const float& AdditiveScore);

	// 해당 팀의 점수를 받아옵니다.
	float GetTeamScore(const EPlayerTeamState& Team) const;

	const float& GetMaxScore() const { return MaxScore; }

	UFUNCTION()
	const uint8& GetMaxPlayers() const { return MaxPlayers; }

	UFUNCTION()
	const uint8& GetNumPlayers() const { return NumPlayers; }

	UFUNCTION()
	const EPlayerTeamState& GetOccupationWinner() const { return CurrentOccupationWinner; }

	// 현재 시간을 기준으로 매칭 시간을 설정합니다. 반드시 서버에서 호출해야 합니다.
	void SetMatchTime();

	// 남은 매칭 시간을 가져옵니다. 아직 매칭시간 정보가 설정되지 않았거나, 시간이 지나간 경우 0을 반환합니다.
	float GetRemainMatchTime() const;

	// 어떤 팀이든 최대 점수에 도달한 팀이 있는지 여부를 조사합니다.
	bool IsSomeoneReachedMaxScore() const;

private:
	UFUNCTION()
	void OnRep_NumPlayers();

	UFUNCTION()
	void OnRep_ATeamScore();

	UFUNCTION()
	void OnRep_BTeamScore();

	UFUNCTION()
	void OnRep_OccupationWinner();

public:
	FOnOccupationChangeJoinedPlayers OnOccupationChangeJoinedPlayers;
	FOnOccupationChangeOccupationWinner OnOccupationChangeOccupationWinner;
	FTeamScoreSignature OnTeamScoreChanged;
	FKillCharacterSignature OnKillCharacterNotify;

private:
	UPROPERTY(ReplicatedUsing = OnRep_NumPlayers)
	uint8 NumPlayers;

	UPROPERTY(ReplicatedUsing = OnRep_OccupationWinner, Transient)
	EPlayerTeamState CurrentOccupationWinner;

	UPROPERTY(ReplicatedUsing = OnRep_ATeamScore, Transient)
	float ATeamScore;

	UPROPERTY(ReplicatedUsing = OnRep_BTeamScore, Transient)
	float BTeamScore;

	UPROPERTY(Replicated)
	float MatchStartTime;

	UPROPERTY(Replicated)
	float MatchEndingTime;

	UPROPERTY(EditAnywhere)
	uint8 MaxPlayers;

	UPROPERTY(EditAnywhere)
	float MaxScore;

	UPROPERTY(EditAnywhere)
	float MatchDuration;
};
