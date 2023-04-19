// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CollectorPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_FiveParams(FOnPlayerStateChange, FPlayerInfo, int32, int32, int32, int32);

UENUM()
enum class EPlayerTeamState : uint8
{
	None UMETA(DisplayerName = "None"), // 현재 팀 배정을 받지 못한 상태
	A UMETA(DisplayerName = "A"), // A팀인 상태
	B UMETA(DisplayerName = "B"), // B팀인 상태
};

// 점수판 정보 구조체
USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY();
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalScore = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Kills = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OccupationSuccess = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OwnObjectNum = 0;
};

/**
 * 
 */
UCLASS()
class LAKAYA_API ACollectorPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ACollectorPlayerState();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	EPlayerTeamState GetPlayerTeamState() const { return PlayerTeamState; }
	void SetPlayerTeamState(EPlayerTeamState TeamState);

	UFUNCTION(BlueprintCallable)
	const FPlayerInfo& GetPlayerInfo() const { return PlayerInfo; }

	UFUNCTION(BlueprintCallable, Category = "Player Info")
	void AddTotalScore();

	UFUNCTION(BlueprintCallable, Category = "Player Info")
	void AddKills();

	UFUNCTION(BlueprintCallable, Category = "Player Info")
	void AddOccupationSuccess();

	UFUNCTION(BlueprintCallable)
	FString GetPlayername() { return PlayerInfo.PlayerName; }
	
	UFUNCTION(BlueprintCallable)
	int32 GetTotalScore() { return PlayerInfo.TotalScore; }

	UFUNCTION(BlueprintCallable)
	int32 GetKills() { return PlayerInfo.Kills; }

	UFUNCTION(BlueprintCallable)
	int32 GetOccupationSuccess() { return PlayerInfo.OccupationSuccess; }
	
	UFUNCTION(BlueprintCallable)
	int32 GetOwnObjectNum() { return PlayerInfo.OwnObjectNum; }

	UFUNCTION(BlueprintCallable)
	void SetOwnObjectNum(int32 Num);
private:
	UFUNCTION()
	void OnPawnSetCallback(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);
	
	UPROPERTY(ReplicatedUsing = OnRep_BroadCastMyTeam, Transient)
	EPlayerTeamState PlayerTeamState = EPlayerTeamState::None;

	UPROPERTY(ReplicatedUsing = OnRep_ScoreUpdate)
	FPlayerInfo PlayerInfo;

private:
	UFUNCTION()
	void OnRep_BroadCastMyTeam();

	UFUNCTION()
	void OnRep_ScoreUpdate();

	UFUNCTION()
	void OnRep_OwnObjectNum();

private:
	TArray<TArray<FPlayerInfo>> PlayerInfoArray;
	
public:
	FOnPlayerStateChange OnPlayerStateChange;
};