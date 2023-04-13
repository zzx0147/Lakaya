// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LakayaBaseCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "MovableCharacter.generated.h"

DECLARE_EVENT_OneParam(AMovableCharacter, FRunSignature, bool)

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
	void SetRunStateServer(bool IsRunning, const float& Time);

	UFUNCTION()
	void OnRep_IsRunning();

	UFUNCTION()
	void OnWalkSpeedChanged(const float& WalkSpeed);

	UFUNCTION()
	void OnRunSpeedChanged(const float& RunSpeed);

	// 달리기 상태 변경을 시도합니다.
	void SetRunStateClient(const bool& IsRunning);

	// 현재 달리기 상태에 맞게 이동 속도를 변경합니다.
	void ApplySpeedFromStat();

public:
	// 달리기 상태가 변경되면 호출됩니다. true이면 달리는 중임을 나타내며, 그렇지 않으면 걷고 있음을 나타냅니다.
	FRunSignature OnRunStateChanged;

private:
	UPROPERTY(ReplicatedUsing=OnRep_IsRunning)
	bool bIsRunning;

	float RecentRunEventTime;
};
