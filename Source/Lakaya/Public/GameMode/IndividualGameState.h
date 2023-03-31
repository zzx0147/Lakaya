// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IndividualGameMode.h"
#include "GameFramework/GameState.h"
#include "IndividualGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeJoinedPlayers, int32);

UENUM()
enum class EGameState : uint8
{
	StandBy UMETA(DisplayName = "StandBy"), // 다른 플레이어 입장 대기 상태
	// SelectWait UMETA(DisplayName = "SelectWait"), // 무기 및 캐릭터 선택 대기 상태
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
	void SetGameState(EGameState NewGameState);

	UPROPERTY(ReplicatedUsing = OnRep_NumPlayers)
	int32 NumPlayers;

	UPROPERTY(ReplicatedUsing = OnRep_GameState)
	EGameState CurrentGameState = EGameState::StandBy;
	
private:
	UFUNCTION()
	void OnRep_NumPlayers();

	UFUNCTION()
	void OnRep_GameState();

	uint8 MaxPlayers = 6;
public:
	FOnChangeJoinedPlayers OnChangeJoinedPlayers;
	// AIndividualGameMode* GameMode;
};