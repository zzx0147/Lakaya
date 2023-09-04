// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OccupationGameState.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "OccupationGameMode.generated.h"

UCLASS()
class LAKAYA_API AOccupationGameMode : public ALakayaDefaultPlayGameMode
{
	GENERATED_BODY()

public:
	AOccupationGameMode();

	virtual void OnPlayerKilled(AController* VictimController, AController* InstigatorController, AActor* DamageCauser) override;

protected:
	// 함수에 대한 설명은 부모클래스에 설명되어 있음.
	virtual void BeginPlay() override;
	virtual void HandleMatchHasEnded() override;
	virtual void RegisterPlayer(AController* NewPlayer) override;
	virtual void HandleMatchHasStarted() override;
	// TODO : 더이상 캐릭터 선택상태는 존재하지 않습니다.
	// virtual void HandleMatchIsSelectCharacter() override;
	
private:
	float MatchStartDelay;
	FTimerHandle TimerHandle_DelayedMatchStart;
	TObjectPtr<AOccupationGameState> OccupationGameState;
};