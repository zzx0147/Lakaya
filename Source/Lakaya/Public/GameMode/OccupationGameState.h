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
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOccupationChangeATeamScore, uint8);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnOccupationChangeBTeamScore, uint8);

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
	void SetMinSec();

	UFUNCTION()
	void SetOccupationObject(EOccupationObjectState NewObjectState);

	UFUNCTION()
	void SetATeamScore();

	UFUNCTION()
	void SetBTeamScore();
	
	UPROPERTY(ReplicatedUsing = OnRep_NumPlayers)
	int32 NumPlayers;

	UPROPERTY(ReplicatedUsing = OnRep_GameState)
	EOccupationGameState CurrentGameState = EOccupationGameState::Menu;

	UPROPERTY(ReplicatedUsing = OnRep_OccupationObjectState)
	EOccupationObjectState CurrentOccupationObjectState = EOccupationObjectState::None;

	UFUNCTION()
	int32 GetMin() { return Min; }

	UFUNCTION()
	int32 GetSec() { return Sec; }

	UFUNCTION()
	uint8 GetATeamScore() { return ATeamScore; }

	UFUNCTION()
	uint8 GetBTeamScore() { return BTeamScore; }
private:
	UPROPERTY(ReplicatedUsing = OnRep_Min)
	int32 Min = 3;

	UPROPERTY(ReplicatedUsing = OnRep_Sec)
	int32 Sec = 0;

	UPROPERTY(ReplicatedUsing = OnRep_ATeamScore)
	uint8 ATeamScore = 0;

	UPROPERTY(ReplicatedUsing = OnRep_BTeamScore)
	uint8 BTeamScore = 0;
	

private:
	UFUNCTION()
	void OnRep_NumPlayers();

	UFUNCTION()
	void OnRep_GameState();

	UFUNCTION()
	void OnRep_Min();
	
	UFUNCTION()
	void OnRep_Sec();

	UFUNCTION()
	void OnRep_OccupationObjectState();

	UFUNCTION()
	void OnRep_ATeamScore();

	UFUNCTION()
	void OnRep_BTeamScore();
	
	UPROPERTY(EditAnywhere)
	uint8 MaxPlayers = 2;
	
public:
	FOnOccupationChangeJoinedPlayers OnOccupationChangeJoinedPlayers;
	FOnOccupationChangeGameState OnOccupationChangeGameState;
	FOnOccupationChangeTime OnOccupationChangeTime;
	FOnOccupationChangeObjectcState OnOccupationChangeObjectcState;
	FOnOccupationChangeATeamScore OnOccupationChangeATeamScore;
	FOnOccupationChangeBTeamScore OnOccupationChangeBTeamScore;

private:
	FTimerHandle TimerHandle_AteamScoreIncrease;
	FTimerHandle TimerHandle_BteamScoreIncrease;
};