// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LakayaBaseCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "MovableCharacter.generated.h"

UCLASS()
class LAKAYA_API AMovableCharacter : public ALakayaBaseCharacter
{
	GENERATED_BODY()

public:
	AMovableCharacter();

	virtual void Crouch(bool bClientSimulation = false) override;

protected:
	virtual void SetupCharacterServer(const FCharacterSetupData* Data) override;

	virtual void OnRep_StatComponent() override;

public:
	void Run();
	void StopRun();

protected:
	// 현재시점의 서버 시간을 가져옵니다.
	float GetServerTime() const { return GetWorld()->GetGameState()->GetServerWorldTimeSeconds(); }

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestSetRunState(bool IsRunning, const float& Time);

	UFUNCTION()
	void OnRep_IsRunning();
	
	void SetRunState(const bool& IsRunning);

	void ApplySpeedFromStat();


	UPROPERTY(ReplicatedUsing=OnRep_IsRunning)
	bool bIsRunning;

	float RecentRunEventTime;
};
