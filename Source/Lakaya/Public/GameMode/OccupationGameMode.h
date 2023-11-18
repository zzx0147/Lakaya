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
	virtual void BeginPlay() override;
	virtual void HandleMatchHasEnded() override;
	virtual void HandleMatchIsSelectCharacter() override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

private:
	// TODO : 테스트용 함수입니다. 나중에 삭제해야 합니다.
	void AssignTeams(const uint8 PlayerCount) const;
	
private:
	UPROPERTY()
	TObjectPtr<AOccupationGameState> OccupationGameState;
};