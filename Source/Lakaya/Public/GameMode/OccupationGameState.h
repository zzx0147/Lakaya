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

private:
	virtual void BeginPlay() override;
	virtual void HandleMatchHasStarted() override;
	
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
	void AddTeamScore(const EPlayerTeam& Team, const float& AdditiveScore);
	
public:
	// 해당 팀의 점수를 받아옵니다.
	float GetTeamScore(const EPlayerTeam& Team) const;

	// 최대 점수를 가져옵니다.
	FORCEINLINE const float& GetMaxScore() const { return MaxScore; }

	// 승리자의 정보를 가져옵니다.
	FORCEINLINE const EPlayerTeam& GetOccupationWinner() const { return CurrentOccupationWinner; }

	// 어떤 팀이든 최대 점수에 도달한 팀이 있는지 여부를 조사합니다.
	FORCEINLINE const bool GetSomeoneReachedMaxScore() const { return ATeamScore >= MaxScore || BTeamScore >= MaxScore; }

private:
	UFUNCTION()
	void OnRep_ATeamScore();

	UFUNCTION()
	void OnRep_BTeamScore();

	UFUNCTION()
	void OnRep_OccupationWinner();
	
private:
	uint8 NumPlayers;

	UPROPERTY(ReplicatedUsing = OnRep_OccupationWinner)
	EPlayerTeam CurrentOccupationWinner;

	UPROPERTY(ReplicatedUsing = OnRep_ATeamScore)
	float ATeamScore = 0;

	UPROPERTY(ReplicatedUsing = OnRep_BTeamScore)
	float BTeamScore = 0;

	UPROPERTY(EditAnywhere)
	float MaxScore;

private:
	// 게임중에 표시되는 팀 스코어 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UTeamScoreWidget> TeamScoreWidgetClass;
	
	// 팀스코어 위젯 입니다.
	TObjectPtr<UTeamScoreWidget> TeamScoreWidget;

public:
	FOnOccupationChangeOccupationWinner OnOccupationChangeOccupationWinner;
	FTeamScoreSignature OnTeamScoreSignature;
	FKillCharacterSignature OnKillCharacterNotify;
};