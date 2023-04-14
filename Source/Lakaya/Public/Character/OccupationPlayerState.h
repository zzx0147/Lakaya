// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "OccupationPlayerState.generated.h"

UENUM()
enum class EPlayerTeamState : uint8
{
	// 팀 배정을 받지 못한 상태
	None UMETA(DisplayerName = "None"),

	// A팀인 상태
	A UMETA(DisplayerName = "A"),

	// B팀인 상태
	B UMETA(DisplayerName = "B"),
};

/**
 * 
 */
UCLASS()
class LAKAYA_API AOccupationPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	const EPlayerTeamState& GetPlayerTeamState() const { return PlayerTeamState; }

	/**
	 * @brief 플레이어의 팀을 설정합니다. 이 함수는 반드시 서버측에서만 호출되어야 합니다.
	 * @param TeamState 설정하려는 팀입니다.
	 */
	void SetPlayerTeamState(const EPlayerTeamState& TeamState);

	// 대상 액터가 같은 팀인지 판별합니다.
	bool IsSameTeam(AActor* const& Other) const;

	// 대상 플레이어 스테이트가 같은 팀인지 판별합니다.
	bool IsSameTeam(APlayerState* const& Other) const;

	// 대상 플레이어 스테이트가 같은 팀인지 판별합니다.
	bool IsSameTeam(AOccupationPlayerState* const& Other) const;

protected:
	UFUNCTION()
	virtual void OnRep_PlayerTeamState();

	UFUNCTION()
	virtual void OnPawnSetCallback(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

private:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerTeamState, Transient)
	EPlayerTeamState PlayerTeamState = EPlayerTeamState::None;
};
