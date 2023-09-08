#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "AgonesComponent.h"
#include "LakayaBaseGameState.h"
#include "LakayaDefaultPlayGameMode.generated.h"

UCLASS()
class LAKAYA_API ALakayaDefaultPlayGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	const static FString ATeamSpawnTag;
	const static FString BTeamSpawnTag;

public:
	ALakayaDefaultPlayGameMode();
	virtual void RestartPlayer(AController* NewPlayer) override;

public:
	virtual void InitStartSpot_Implementation(AActor* StartSpot, AController* NewPlayer) override;
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;
	virtual UAgonesComponent* GetAgonesSDK() { return AgonesSDK; }

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	// 플레이어가 로그인 할 경우 호출
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 플레이어가 로그아웃 할 경우 호출
	virtual void Logout(AController* Exiting) override;

	//SetMatchState()가 호출되서 MatchState가 변경된 직후 호출됨, HandleMatchIsWaitingToStart() 같은 MatchState에 맞는 HandleMatch~~ 함수를 호출함
	virtual void OnMatchStateSet() override;
	
	// EnteringMap (맵 진입)
	// 액터 틱은 아직 이루어지지 않으며, 월드는 제대로 초기화 되지 않은 상태.
	// 모두 완전히 로드되면 다음 상태로 전환.

	// WaitingToStart (시작 대기 중)
	// 여기에 들어갈 때 HandleMatchIsWaitingToStart()함수 호출
	// 경기 시작 대기 중 처리. (액터 틱은 이루어지지만, 플레이어는 아직 스폰 되어 있지 않음)
	// ReadyToStartMatch (경기 시작 준비)가 true를 반환하는 경우, 또는 StartMatch가 호출된 경우 다음 상태로 전환.
	virtual void HandleMatchIsWaitingToStart() override;
	virtual bool ReadyToStartMatch_Implementation() override;

	// 캐릭터 선택 스테이스로 넘어갈 때 호출되는 함수
	// virtual void HandleMatchIsSelectCharacter();

	// InProgress (진행중)
	// 여기에 들어갈 때 HandleMatchHasStarted()함수 호출
	// 경기 시작 처리. (모든 액터에서 BeginPlay를 호출)
	// ReadyToEndMatch (경기 종료 처리)가 true로 반환하거나, EndMath(경기 종료)가 호출되는 경우 경기가 다음 상태로 전환.
	virtual void HandleMatchHasStarted() override;

	// WaitingPostMath (경기 후 대기)
	// 여기에 들어설 때 HandleMatchHasEnded ()함수 호출
	// 액터 틱은 여전히 일어나지만, 새로운 플레이어는 참가할 수 없음. 맵 이동이 시작되면 다음 상태로 전환
	virtual void HandleMatchHasEnded() override;

	// LeavingMap (맵 떠나기)
	// 여기에 들어설 때 HandleLeavingMap()함수 호출
	// 일반적인 흐름의 마지막 상태.
	virtual void HandleLeavingMap() override;
	
public:
	virtual void OnPlayerKilled(AController* VictimController, AController* InstigatorController, AActor* DamageCauser);
	virtual bool HasMatchStarted() const override;

protected:
	virtual void RespawnPlayer(AController* KilledController);
	virtual bool ShouldRespawn();
	virtual void RegisterPlayer(AController* NewPlayer); //플레이어 혹은 AI 접속시 이벤트 바인딩 및 초기화 등을 실행합니다
	virtual void DelayedEndedGame();
	
public:
	UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<FName, TSubclassOf<class AInteractableCharacter>> CharacterClasses;

	FTimerHandle TimerHandle_DelayedEnded;
	float MatchEndDelay = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAgonesComponent* AgonesSDK;

	float MatchStartDelay;
protected:
	uint8 CurrentPlayerNum;
	TObjectPtr<ALakayaBaseGameState> BaseGameState;
	FTimerHandle TimerHandle_DelayedMatchStart;
	
private:
	UPROPERTY()
	TMap<AController*, FTimerHandle> RespawnTimers;
};