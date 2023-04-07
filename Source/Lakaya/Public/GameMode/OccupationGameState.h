// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OccupationGameMode.h"
#include "GameFramework/GameState.h"
#include "OccupationGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnOccupationChangeJoinedPlayers, int32, int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOccupationChangeGameState, EOccupationGameState)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnOccupationChangeTime, int32, int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOccupationChangeObjectcState, EOccupationObjectState)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOccupationChangeATeamScore, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOccupationChangeBTeamScore, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOccupationChangeATeamObjectNum, uint8);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOccupationChangeBTeamObjectNum, uint8);

UENUM()
enum class EOccupationGameState : uint8
{
	Menu UMETA(DisplayerName = "Menu"), // 메뉴 선택, 설정 ... 상태
	StandBy UMETA(DisplayName = "StandBy"), // 다른 플레이어 입장 대기 상태
	// SelectWait UMETA(DisplayName = "SelectWait"), // 무기 및 캐릭터 선택 대기 상태
	StandByToPregressLoading UMETA(DisplayName = "StandByToPregressLoading"),
	Progress UMETA(DisplayName = "Progress"), // 게임진행 상태
	Finish UMETA(DisplayName = "Finish") // 게임종료 상태
};

UENUM()
enum class EOccupationObjectState : uint8
{
	None UMETA(DisplayerName = "None"), // 어느 팀에서도 활성화 되어 있지 않은 상태.
	A UMETA(DisplayerName = "A"), // A팀에서의 활성화 상태
	B UMETA(DisplayerName = "B") // B팀에서의 활성화 상태
};

/**
 * 
 */
UCLASS()
class LAKAYA_API AOccupationGameState : public AGameState
{
	GENERATED_BODY()

private:
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
	uint8 GetMaxPlayers() const { return MaxPlayers; }

	UFUNCTION()
	void SetNumPlayers(int32 NewNumPlayers);

	UFUNCTION()
	void SetGameState(EOccupationGameState NewGameState);
	
	UFUNCTION()
	void SetOccupationObject(EOccupationObjectState NewObjectState);

	UFUNCTION()
	void SetATeamScore();

	UFUNCTION()
	void SetBTeamScore();

	UFUNCTION()
	void AddATeamObjectNum();

	UFUNCTION()
	void AddBTeamObjectNum();

	UFUNCTION()
	void SubATeamObjectNum();

	UFUNCTION()
	void SubBTeamObjectNum();
	
	UPROPERTY(ReplicatedUsing = OnRep_NumPlayers)
	int32 NumPlayers;

	UPROPERTY(ReplicatedUsing = OnRep_GameState)
	EOccupationGameState CurrentGameState = EOccupationGameState::Menu;

	UPROPERTY(ReplicatedUsing = OnRep_OccupationObjectState)
	EOccupationObjectState CurrentOccupationObjectState = EOccupationObjectState::None;
	
	UFUNCTION()
	float GetATeamScore() { return ATeamScore; }

	UFUNCTION()
	float GetBTeamScore() { return BTeamScore; }

	UFUNCTION()
	float GetATeamObjectNum() { return ATeamObjectNum; }

	UFUNCTION()
	float GetBTeamObjectNum() { return BTeamObjectNum; }

	/**
	 * @brief 매치가 시작되었을 때 호출됩니다. 
	 * @param MatchTime 매치가 몇초동안 진행되는지 나타냅니다.
	 */
	void OnMatchStarted(const float& MatchTime);

	// 남은 매칭 시간을 가져옵니다. 아직 매칭시간 정보가 설정되지 않았거나, 시간이 지나간 경우 0을 반환합니다.
	float GetRemainMatchTime();
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_ATeamScore)
	float ATeamScore = 0;

	UPROPERTY(ReplicatedUsing = OnRep_BTeamScore)
	float BTeamScore = 0;

	UPROPERTY(Replicated)
	float StartTime;

	UPROPERTY(Replicated)
	float MatchEndingTime;

	float Standard = 0.2f;

	UPROPERTY(ReplicatedUsing = OnRep_ATeamObjectNum)
	uint8 ATeamObjectNum = 0;

	UPROPERTY(ReplicatedUsing = OnRep_BTeamObjectNum)
	uint8 BTeamObjectNum = 0;
	
private:
	UFUNCTION()
	void OnRep_NumPlayers();

	UFUNCTION()
	void OnRep_GameState();

	UFUNCTION()
	void OnRep_OccupationObjectState();

	UFUNCTION()
	void OnRep_ATeamScore();

	UFUNCTION()
	void OnRep_BTeamScore();

	UFUNCTION()
	void OnRep_ATeamObjectNum();

	UFUNCTION()
	void OnRep_BTeamObjectNum();
	
	UPROPERTY(EditAnywhere)
	uint8 MaxPlayers = 2;
	
public:
	FOnOccupationChangeJoinedPlayers OnOccupationChangeJoinedPlayers;
	FOnOccupationChangeGameState OnOccupationChangeGameState;
	FOnOccupationChangeTime OnOccupationChangeTime;
	FOnOccupationChangeObjectcState OnOccupationChangeObjectcState;
	FOnOccupationChangeATeamScore OnOccupationChangeATeamScore;
	FOnOccupationChangeBTeamScore OnOccupationChangeBTeamScore;
	FOnOccupationChangeATeamObjectNum OnOccupationChangeATeamObjectNum;
	FOnOccupationChangeBTeamObjectNum OnOccupationChangeBTeamObjectNum;
	
private:
	FTimerHandle TimerHandle_AteamScoreIncrease;
	FTimerHandle TimerHandle_BteamScoreIncrease;
};