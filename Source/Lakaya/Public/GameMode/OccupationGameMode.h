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
	virtual void HandleMatchIsSelectCharacter() override;

private:
	// TODO : 테스트용 함수입니다. 나중에 삭제해야 합니다.
	void AssignTeams(const uint8 PlayerCount) const;
	
private:
	TObjectPtr<AOccupationGameState> OccupationGameState;
};