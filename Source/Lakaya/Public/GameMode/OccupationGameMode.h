// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "Occupation/PlayerTeam.h"
#include "OccupationGameMode.generated.h"

UCLASS()
class LAKAYA_API AOccupationGameMode : public ALakayaDefaultPlayGameMode
{
	GENERATED_BODY()

public:
	AOccupationGameMode();

protected:
	// 함수에 대한 설명은 부모클래스에 설명되어 있음.
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual bool ReadyToStartMatch_Implementation() override;
	virtual bool ReadyToEndMatch_Implementation() override;
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
	virtual void HandleMatchIsSelectCharacter() override;

protected:
	virtual void RespawnPlayer(AController* KilledController) override;
	virtual void PlayerInitializeSetLocation(uint8 PlayersNum) override;
	virtual void OnKilledCharacter(AController* VictimController, AActor* Victim, AController* InstigatorController, AActor* DamageCauser) override;

public:
	/**
	 * @brief 팀이 점령중인 오브젝트 개수를 추가합니다.
	 * @param Team 점령을 시도한 팀입니다.
	 */
	void AddOccupyObject(const EPlayerTeam& Team);

	/**
	 * @brief 팀이 점령중인 오브젝트 개수를 차감합니다.
	 * @param Team 점령을 해제당한 팀입니다.
	 */
	void SubOccupyObject(const EPlayerTeam& Team);

private:
	// 일정시간마다 호출되어 각 팀에 점수를 부여합니다.
	void UpdateTeamScoreTick();

	// 1초마다 점수가 오를지를 나타냅니다.
	UPROPERTY(EditAnywhere)
	float ScoreUpdateDelay;

	// 오브젝트 한 개당 부여될 점수입니다.
	UPROPERTY(EditAnywhere)
	float AdditiveScore;

	class AOccupationGameState* OccupationGameState;

	FTimerHandle UpdateScoreTimer;
	
	uint8 ATeamObjectCount;
	uint8 BTeamObjectCount;
};