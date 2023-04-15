// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Occupation/PlayerTeamState.h"
#include "OccupationGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnOccupationChangeJoinedPlayers, const uint8&)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOccupationChangeOccupationWinner, const EPlayerTeamState&)
DECLARE_EVENT_TwoParams(AOccupationGameState, FOnTeamScoreChanged, const EPlayerTeamState&, const float&)


/**
 * 
 */
UCLASS()
class LAKAYA_API AOccupationGameState : public AGameState
{
	GENERATED_BODY()

	const static float MaxScore;

public:
	// 하드코딩으로 설정된 최대 점수를 가져옵니다.
	static const float& GetMaxScore() { return MaxScore; }

	UFUNCTION()
	void SetNumPlayers(const uint8& NewNumPlayers);

	UFUNCTION()
	void SetOccupationWinner(const EPlayerTeamState& Winner);

	/**
	 * @brief 팀에 점수를 부여합니다.
	 * @param Team 점수를 부여할 팀입니다.
	 * @param AdditiveScore 추가될 점수입니다.
	 */
	void AddTeamScore(const EPlayerTeamState& Team, const float& AdditiveScore);

	// 해당 팀의 점수를 받아옵니다.
	const float& GetTeamScore(const EPlayerTeamState& Team) const;

	UFUNCTION()
	const uint8& GetMaxPlayers() const { return MaxPlayers; }

	UFUNCTION()
	const uint8& GetNumPlayers() const { return NumPlayers; }

	UFUNCTION()
	const EPlayerTeamState& GetOccupationWinner() const { return CurrentOccupationWinner; }

	/**
	 * @brief 현재 시간을 기준으로 매칭 시간을 설정합니다. 반드시 서버에서 호출해야 합니다.
	 * @param MatchTime 매치가 몇초동안 진행되는지 나타냅니다.
	 */
	void SetMatchTime(const float& MatchTime);

	// 남은 매칭 시간을 가져옵니다. 아직 매칭시간 정보가 설정되지 않았거나, 시간이 지나간 경우 0을 반환합니다.
	float GetRemainMatchTime();

private:
	UFUNCTION()
	void OnRep_NumPlayers();

	UFUNCTION()
	void OnRep_ATeamScore();

	UFUNCTION()
	void OnRep_BTeamScore();

	UFUNCTION()
	void OnRep_OccupationWinner();

	UPROPERTY(ReplicatedUsing = OnRep_NumPlayers)
	uint8 NumPlayers;

	UPROPERTY(ReplicatedUsing = OnRep_OccupationWinner)
	EPlayerTeamState CurrentOccupationWinner;

	UPROPERTY(ReplicatedUsing = OnRep_ATeamScore)
	float ATeamScore = 0;

	UPROPERTY(ReplicatedUsing = OnRep_BTeamScore)
	float BTeamScore = 0;

	UPROPERTY(Replicated)
	float StartTime;

	UPROPERTY(Replicated)
	float MatchEndingTime;

	UPROPERTY(EditAnywhere)
	uint8 MaxPlayers = 2;

public:
	FOnOccupationChangeJoinedPlayers OnOccupationChangeJoinedPlayers;
	FOnTeamScoreChanged OnTeamScoreChanged;
	FOnOccupationChangeOccupationWinner OnOccupationChangeOccupationWinner;
};
