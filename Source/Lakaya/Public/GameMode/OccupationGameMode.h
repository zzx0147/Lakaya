// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/LakayaDefalutPlayGameMode.h"
#include "OccupationGameMode.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGameModeInitialized);

/**
 * 
 */
UCLASS()
class LAKAYA_API AOccupationGameMode : public ALakayaDefalutPlayGameMode
{
	GENERATED_BODY()

private:
	AOccupationGameMode();

	// 함수에 대한 설명은 부모클래스에 설명되어 있음.
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void HandleMatchIsWaitingToStart() override;
	virtual bool ReadyToStartMatch_Implementation() override;
	virtual void DelayedStartMatch() override;
	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
	virtual void HandleLeavingMap() override;
	virtual void Logout(AController* Exiting) override;

private:
	virtual void OnKilledCharacter(AController* VictimController, AActor* Victim, AController* InstigatorController, AActor* DamageCauser) override;
	
	virtual void OnKillNotifyBinding() override;

	virtual void RespawnPlayer(AController* KilledController) override;

private:
	FTimerHandle TimerHandle_CheckStartMatch;
	FTimerHandle TimerHandle_DelayedStart;

public:
	FOnGameModeInitialized OnGameModeInitialized;
};