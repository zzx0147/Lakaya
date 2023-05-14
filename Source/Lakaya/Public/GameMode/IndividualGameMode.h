// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"
#include "IndividualGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API AIndividualGameMode : public ALakayaDefaultPlayGameMode
{
	GENERATED_BODY()

public:
	AIndividualGameMode();

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

	
};
