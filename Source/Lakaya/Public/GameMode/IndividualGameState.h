// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IndividualGameMode.h"
#include "GameFramework/GameState.h"
#include "IndividualGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnIndividualChangeJoinedPlayers, int32, int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnIndividualChangeGameState, EIndividualGameState)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnIndividualChangeTime, int32, int32)

UENUM()
enum class EIndividualGameState : uint8
{
	Menu UMETA(DisplayerName = "Menu"), // 메뉴 선택, 설정 ... 상태
	StandBy UMETA(DisplayName = "StandBy"), // 다른 플레이어 입장 대기 상태
	// SelectWait UMETA(DisplayName = "SelectWait"), // 무기 및 캐릭터 선택 대기 상태
	StandByToPregressLoading UMETA(DisplayName = "StandByToPregressLoading"),
	Progress UMETA(DisplayName = "Progress"), // 게임진행 상태
	Finish UMETA(DisplayName = "Finish") // 게임종료 상태
};

UCLASS()
class LAKAYA_API AIndividualGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UFUNCTION()
	uint8 GetMaxPlayers() const { return MaxPlayers; }

	UFUNCTION()
	void SetNumPlayers(int32 NewNumPlayers);

	UFUNCTION()
	void SetGameState(EIndividualGameState NewGameState);

	UFUNCTION()
	void SetMinSec();
	
	UPROPERTY(ReplicatedUsing = OnRep_NumPlayers)
	int32 NumPlayers;

	UPROPERTY(ReplicatedUsing = OnRep_GameState)
	EIndividualGameState CurrentGameState = EIndividualGameState::Menu;

	UFUNCTION()
	int32 GetMin() {return Min; }

	UFUNCTION()
	int32 GetSec() {return Sec; }
private:
	UPROPERTY(ReplicatedUsing = OnRep_Min)
	int32 Min = 3;
	UPROPERTY(ReplicatedUsing = OnRep_Sec)
	int32 Sec = 0;
	
private:
	UFUNCTION()
	void OnRep_NumPlayers();

	UFUNCTION()
	void OnRep_GameState();

	UFUNCTION()
	void OnRep_Min();
	
	UFUNCTION()
	void OnRep_Sec();

	UPROPERTY(EditAnywhere)
	uint8 MaxPlayers = 2;

public:
	FOnIndividualChangeJoinedPlayers OnIndividualChangeJoinedPlayers;
	FOnIndividualChangeGameState OnIndividualChangeGameState;
	FOnIndividualChangeTime OnIndividualChangeTime;
};