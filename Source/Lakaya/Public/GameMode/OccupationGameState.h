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

protected:
	virtual void AddPlayerState(APlayerState* PlayerState) override;

public:
	// 해당 팀의 점수를 받아옵니다.
	float GetTeamScore(const EPlayerTeam& Team) const;

	// 최대 점수를 가져옵니다.
	FORCEINLINE const float& GetMaxScore() const { return MaxScore; }

	// 승리자의 정보를 가져옵니다.
	FORCEINLINE const EPlayerTeam& GetOccupationWinner() const { return CurrentOccupationWinner; }

	// 어떤 팀이든 최대 점수에 도달한 팀이 있는지 여부를 조사합니다.
	bool IsSomeoneReachedMaxScore() const;

	virtual void CreateCharacterSelectWidget(APlayerController* LocalController) override;

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

	UPROPERTY(EditAnywhere)
	float MaxScore;


	TMap<EPlayerTeam,TArray<class ALakayaBasePlayerState*>> PlayersByTeamMap;

	EPlayerTeam ClientTeam;
};
