// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/OccupationGameState.h"
#include "GameMode/LakayaDefalutPlayGameMode.h"
#include "Character/DamageableCharacter.h"
#include "OccupationGameMode.generated.h"

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

	void PlayerInitializeSetLocation(const uint8 PlayersNum);

	void DelayedEndedGame();

private:
	UPROPERTY(EditAnywhere)
	float GamePlayTime = 180.0f;
	
	AOccupationGameState* OccupationGameState;
	TArray<ADamageableCharacter*> BoundActors;

private:
	FTimerHandle TimerHandle_CheckStartMatch;
	FTimerHandle TimerHandle_DelayedStart;
	FTimerHandle TimerHandle_DelayedEnded;
};