#pragma once

#include "EngineMinimal.h"
#include "LakayaDefalutPlayGameMode.h"
#include "IndividualGameMode.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGameModeInitialized);

UCLASS()
class LAKAYA_API AIndividualGameMode : public ALakayaDefalutPlayGameMode
{
	GENERATED_BODY()

private:
	AIndividualGameMode();
	
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	// 플레이어가 로그인 할 경우 호출
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// EnteringMap (맵 진입)
	// 액터 틱은 아직 이루어지지 않으며, 월드는 제대로 초기화 되지 않은 상태.
	// 모두 완전히 로드되면 다음 상태로 전환.
	
	// WaitingToStart (시작 대기 중)
	// 여기에 들어갈 때 HandleMatchIsWaitingToStart()함수 호출
	// 경기 시작 대기 중 처리. (액터 틱은 이루어지지만, 플레이어는 아직 스폰 되어 있지 않음)
	// ReadyToStartMatch (경기 시작 준비)가 true를 반환하는 경우, 또는 StartMatch가 호출된 경우 다음 상태로 전환.
	virtual void HandleMatchIsWaitingToStart() override;
	virtual bool ReadyToStartMatch_Implementation() override;
	// virtual void StartMatch() override;
	void DelayedStartMatch();
	
	// InProgress (진행중)
	// 여기에 들어갈 때 HandleMatchHasStarted()함수 호출
	// 경기 시작 처리. (모든 액터에서 BeginPlay를 호출)
	// ReadyToEndMatch (경기 종료 처리)가 true로 반환하거나, EndMath(경기 종료)가 호출되는 경우 경기가 다음 상태로 전환.
	virtual void HandleMatchHasStarted() override;
	
 	// WaitingPostMath (경기 후 대기)
	// 여기에 들어설 때 HandleLeavingMap()함수 호출
	// 액터 틱은 여전히 일어나지만, 새로운 플레이어는 참가할 수 없음. 맵 이동이 시작되면 다음 상태로 전환
	virtual void HandleMatchHasEnded() override;

	// LeavingMap (맵 떠나기)
	// 여기에 들어설 때 HandleLeavingMap()함수 호출
	// 일반적인 흐름의 마지막 상태.
	virtual void HandleLeavingMap() override;

	// 플레이어가 로그아웃 할 경우 호출
	virtual void Logout(AController* Exiting) override;

private:
	UFUNCTION()
	void OnKilledCharacter(AController* VictimController, AActor* Victim, AController* InstigatorController, AActor* DamageCauser);
	
	void OnKillNotifyBinding();

	void RespawnPlayer(AController* Controller);

	void SpawnDropEnergy(AController* DeadPlayer);

public:
	uint8 PlayerRespawnTime = 3;
	bool bWaitToStart = false;
	
public:
	UPROPERTY()
	TSet<APlayerController*> RegisteredPlayers;

private:
	UPROPERTY()
	TMap<AController*, FTimerHandle> RespawnTimers;

private:
	FTimerHandle TimerHandle_SpawnStaticEnergy;
	FTimerHandle TimerHandle_CheckStartMatch;
	FTimerHandle TimerHandle_DelayedStart;
	
public:
	FOnGameModeInitialized OnGameModeInitialized;
};