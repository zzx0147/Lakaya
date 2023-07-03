// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Occupation/PlayerTeam.h"
#include "TimerManager.h"
#include "EOS/EOSGameInstance.h"
#include "LakayaBasePlayerState.generated.h"

DECLARE_EVENT_OneParam(ALakayaBasePlayerState, FHealthChangeSignature, const float&)

DECLARE_EVENT_OneParam(ALakayaBasePlayerState, FTeamSignature, const EPlayerTeam&)

DECLARE_EVENT_ThreeParams(ALakayaBasePlayerState, FPlayerKillSignature, AController*, AController*, AActor*)

DECLARE_EVENT_OneParam(ALakayaBasePlayerState, FAliveChangeSignature, bool)

DECLARE_EVENT_TwoParams(ALakayaBasePlayerState, FCharacterNameChangeSignature, ALakayaBasePlayerState*, const FName&)

DECLARE_EVENT_OneParam(ALakayaBasePlayerState, FCountInfoSignature, const uint16&)

DECLARE_EVENT_OneParam(ALakayaBasePlayerState, FPlayerNameSignature, const FString&)

DECLARE_EVENT_OneParam(ALakayaBasePlayerState, FOwnerChangeSignature, AActor*)

DECLARE_DELEGATE_OneParam(FRespawnTimerDelegate, AController*)

UCLASS()
class LAKAYA_API ALakayaBasePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ALakayaBasePlayerState();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	virtual void OnRep_PlayerName() override;
	virtual void SetOwner(AActor* NewOwner) override;

protected:
	virtual void BeginPlay() override;
	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OnRep_Owner() override;

public:
	bool IsSameTeam(const ALakayaBasePlayerState* Other) const;

	UFUNCTION(BlueprintGetter)
	bool IsSameTeam(const EPlayerTeam& Other) const;

	// 플레이어의 팀을 설정합니다.
	virtual void SetTeam(const EPlayerTeam& DesireTeam);

	// 플레이어의 현재 팀 정보를 가져옵니다.
	UFUNCTION(BlueprintCallable)
	const EPlayerTeam& GetTeam() const { return Team; }

	/**
	 * @brief 플레이어가 예약된 시간에 부활하도록 합니다.
	 * @param ReservedRespawnTime 목표 부활 시간입니다. 이 시간에 플레이어가 부활합니다.
	 * 음수를 기입하여 기약없이 사망한 상태로, 0을 포함한 현재 시간보다 낮은 값을 기입하여 생존상태로 변경시킬 수도 있습니다.
	 * 이러한 경우 Object의 Function은 호출되지 않습니다.
	 * @param Callback 리스폰 타이머가 종료되면 호출될 콜백입니다.
	 */
	void SetRespawnTimer(const float& ReservedRespawnTime, const FRespawnTimerDelegate& Callback = nullptr);

	// 이 플레이어의 생존 여부를 가져옵니다.
	UFUNCTION(BlueprintGetter)
	const bool& IsAlive() const { return bRecentAliveState; }

	// 이 플레이어를 생존상태로 변경합니다.
	void MakeAlive();

	/**
	 * @brief 서버에게 캐릭터 변경을 요청합니다. 픽창에서는 캐릭터가 실제로 변경되지는 않고 게임이 시작될 때 요청된 캐릭터가 생성되며,
	 * 게임이 진행중일 때는 실제로 캐릭터가 변경됩니다.
	 * @param Name 변경이 요청된 캐릭터 이름입니다. 이 값을 통해 데이터 테이블에서 캐릭터를 찾아 생성할 수 있습니다.
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestCharacterChange(const FName& Name);

	// 현재 플레이어의 점수를 가져옵니다.
	FORCEINLINE const uint16& GetTotalScore() const { return TotalScore; }

	// 현재 플레이어의 점령한 오브젝트 갯수를 가져옵니다.
	FORCEINLINE const uint16& GetCurrentCaptureCount() const { return CurrentCaptureCount; }

	// 현재 플레이어의 누적 점령 성공 횟수를 가져옵니다.
	FORCEINLINE const uint16& GetSuccessCaptureCount() const { return SuccessCaptureCount; }

	// 현재 플레이어의 누적 킬 횟수를 가져옵니다.
	const uint16& GetKillCount() const { return KillCount; }

	// 현재 플레이어의 누적 사망 횟수를 가져옵니다.
	const uint16& GetDeathCount() const { return DeathCount; }

	// 플레이어가 선택한 캐릭터의 이름을 가져옵니다.
	UFUNCTION(BlueprintGetter)
	const FName& GetCharacterName() const { return CharacterName; }

	// 플레이어의 연속처치 횟수를 가져옵니다.
	const uint16& GetKillStreak() const { return KillStreak; }

	// 현재 플레이어의 점수를 올려줍니다.
	const uint16& AddTotalScoreCount(const uint16& NewScore);

	// 플레이어의 누적 점령 성공 횟수를 늘립니다.
	virtual void IncreaseSuccessCaptureCount();

	// 플레이어의 현재 점령한 오브젝트 횟수를 늘립니다.
	virtual void IncreaseCurrentCaptureCount();

	// 플레이어의 현재 점령한 오브젝트 갯수를 줄입니다.
	virtual void DecreaseCurrentCaptureCount();

	// 플레이어의 누적 사망 횟수를 늘립니다.
	virtual void IncreaseDeathCount();

	// 플레이어의 킬 횟수를 늘립니다.
	virtual void IncreaseKillCount();

	// 플레이어의 연속처치 횟수를 늘립니다.
	virtual void IncreaseKillStreak();

	// 플레이어의 연속처치 횟수를 초기화합니다.
	virtual void ResetKillStreak();

	// 플레이어가 점령했을 때, 현재 점령한 오브제그 갯수를 체크를 합니다.
	virtual void CheckCurrentCaptureCount();

	// 이 플레이어가 사용할 고유한 스텐실 마스크 값을 설정합니다.
	virtual void SetUniqueStencilMask(const ERendererStencilMask& StencilMask);

	void SetAlly(const bool& Ally);

	FPlayerStats GetPlayerStats();
	
protected:
	// 현재 서버의 시간을 가져옵니다.
	float GetServerTime() const;

	// OnMaxHealthChanged 이벤트를 호출합니다.
	void BroadcastMaxHealthChanged() const;

	/**
	 * @brief 폰의 ShouldTakeDamage를 모방한 함수입니다. 데미지를 받을지 여부를 판단합니다.
	 * @return true이면 피해를 입고, 그렇지 않으면 피해를 입지 않습니다.
	 */
	virtual bool ShouldTakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                              AActor* DamageCauser);

	/**
	 * @brief 이 플레이어 스테이트의 폰이 변경되는 경우 호출됩니다.
	 * @param Player this와 같으므로 사용할 필요 없습니다.
	 * @param NewPawn 새로 어태치된 폰입니다.
	 * @param OldPawn 이제 어태치가 해제된 폰입니다.
	 */
	UFUNCTION()
	virtual void OnPawnSetCallback(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	// 최대 체력을 가져옵니다.
	virtual float GetMaxHealth() const;

	/**
	 * @brief 캐릭터 이름을 변경할 수 있는지 여부를 조사합니다.
	 * @param Name 변경하려는 이름입니다.
	 * @return true이면 캐릭터 이름을 변경할 수 있는 것으로 간주합니다.
	 */
	virtual bool ShouldChangeCharacterName(const FName& Name);

	UFUNCTION()
	virtual void OnRep_Health();

	UFUNCTION()
	virtual void OnRep_Team();

	UFUNCTION()
	virtual void OnRep_RespawnTime();

	UFUNCTION()
	virtual void OnRep_CharacterName();

	UFUNCTION()
	virtual void OnRep_TotalScore();

	UFUNCTION()
	virtual void OnRep_CurrentCaptureCount();

	UFUNCTION()
	virtual void OnRep_SuccessCaptureCount();

	UFUNCTION()
	virtual void OnRep_DeathCount();

	UFUNCTION()
	virtual void OnRep_KillCount();

	UFUNCTION()
	virtual void OnRep_KillStreak();

private:
	/**
	 * @brief RespawnTime을 통해 생존 상태를 판별하고, 생존상태가 변경되었다면 업데이트합니다.
	 * @param CurrentTime 생존 판별 기준이 될 현재 시간입니다.
	 */
	void UpdateAliveStateWithRespawnTime(const float& CurrentTime);

	// 생존 상태를 강제로 지정합니다. 생존 상태는 기본적으로 RespawnTime을 통해 추론되지만
	// GetServerTime()의 시간오차로 인해 RespawnTimer가 콜백된 시점에
	// RespawnTime을 통해 추론한 생존상태가 여전히 false일 수 있는 문제를 회피하기 위해 만들어졌습니다.
	void SetAliveState(bool AliveState);

	/**
	 * @brief 플레이어가 피격됐음을 오너 클라이언트에게 알려줍니다.
	 * @param CauserName 피해를 입힌 액터의 이름입니다.
	 * @param CauserLocation 피해를 입힌 당시의 액터의 위치입니다.
	 * @param Damage 최종적으로 플레이어가 입은 피해량입니다.
	 */
	UFUNCTION(Client, Reliable)
	void NoticePlayerHit(const FName& CauserName, const FVector& CauserLocation, const float& Damage);

	void RespawnTimerCallback(FRespawnTimerDelegate Callback);

public:
	// 현재 체력이 변경되는 경우 호출됩니다. 매개변수로 변경된 현재 체력을 받습니다.
	FHealthChangeSignature OnHealthChanged;

	// 최대 체력이 변경되는 경우 호출됩니다. 매개변수로 변경된 전체 체력을 받습니다.
	FHealthChangeSignature OnMaxHealthChanged;

	// 현재 팀이 변경되는 경우 호출됩니다. 매개변수로 현재 팀을 받습니다.
	FTeamSignature OnTeamChanged;

	// 플레이어가 사망조건을 달성한 경우 호출됩니다. 매개변수로 살해당한 플레이어의 컨트롤러, 살해한 액터, 살해한 플레이어의 컨트롤러를 받습니다.
	FPlayerKillSignature OnPlayerKilled;

	// 플레이어가 살아나거나 죽는 경우 호출됩니다. 매개변수로 생존 상태를 받습니다. true이면 생존, false이면 사망을 의미합니다.
	FAliveChangeSignature OnAliveStateChanged;

	// 캐릭터 이름이 변경될 때 호출됩니다. 매개변수로 이 플레이어 스테이트와 변경된 캐릭터 이름을 받습니다.
	FCharacterNameChangeSignature OnCharacterNameChanged;

	// 플레이어의 누적 점수가 변경되는 경우 호출됩니다. 매개변수로 변경된 점수를 받습니다.
	FCountInfoSignature OnTotalScoreChanged;

	// 플레이어 현재 점령한 오브젝트 갯수가 변경되는 경우 호출됩니다. 매개변수로 변경된 현재 점령한 오브젝트 갯수를 받습니다.
	FCountInfoSignature OnCurrentCaptureCountChanged;

	// 플레이어 누적 점령 성공 횟수가 변경되는 경우 호출됩니다. 매개변수로 변경된 점령 성공 횟수를 받습니다.
	FCountInfoSignature OnSuccessCaptureCountChanged;

	// 플레이어의 누적 킬 횟수가 변경되는 경우 호출됩니다. 매개변수로 변경된 킬 횟수를 받습니다.
	FCountInfoSignature OnKillCountChanged;

	// 플레이어의 누적 사망 횟수가 변경되는 경우 호출됩니다. 매개변수로 변경된 누적 사망 횟수를 받습니다.
	FCountInfoSignature OnDeathCountChanged;

	// 플레이어의 연속처치 횟수가 변경되는 경우 호출됩니다. 매개변수로 변경된 연속처치 횟수를 받습니다.
	FCountInfoSignature OnKillStreakChanged;

	// 플레이어의 이름이 변경될 때 호출됩니다. 매개변수로 변경된 플레이어의 이름을 받습니다.
	FPlayerNameSignature OnPlayerNameChanged;

	// 오너가 변경될 때 호출됩니다. 매개변수로 변경된 오너의 AActor 포인터를 받습니다.
	FOwnerChangeSignature OnOwnerChanged;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGamePlayHealthWidget> HealthWidgetClass;

	// 게임중에 표시되는 피격 레이더 위젯 클래스를 지정합니다.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UDirectionalDamageIndicator> DirectionDamageIndicatorClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGamePlayPortraitWidget> PortraitWidgetClass;

private:
	UPROPERTY(ReplicatedUsing=OnRep_Health, Transient)
	float Health;

	UPROPERTY(ReplicatedUsing=OnRep_Team, Transient)
	EPlayerTeam Team;

	UPROPERTY(ReplicatedUsing=OnRep_RespawnTime, Transient)
	float RespawnTime;

	UPROPERTY(ReplicatedUsing=OnRep_CharacterName, Transient)
	FName CharacterName;

	// 1Kill = 100 Score
	// CaptureSuccess = 500 Score
	// 1 Second = (CurrentCapturedObject * 50) Score
	UPROPERTY(ReplicatedUsing=OnRep_TotalScore, Transient)
	uint16 TotalScore;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentCaptureCount, Transient)
	uint16 CurrentCaptureCount;

	uint16 OccupationTickCount;
	
	UPROPERTY(ReplicatedUsing=OnRep_SuccessCaptureCount, Transient)
	uint16 SuccessCaptureCount;

	UPROPERTY(ReplicatedUsing=OnRep_DeathCount, Transient)
	uint16 DeathCount;

	UPROPERTY(ReplicatedUsing=OnRep_KillCount, Transient)
	uint16 KillCount;

	UPROPERTY(ReplicatedUsing=OnRep_KillStreak, Transient)
	uint16 KillStreak;

	FTimerHandle RespawnTimer;
	FTimerHandle CurrentCaptureTimer;
	bool bRecentAliveState;
	ERendererStencilMask UniqueRenderMask;
	bool bIsAlly;

	TWeakObjectPtr<UGamePlayHealthWidget> HealthWidget;
	TObjectPtr<UDirectionalDamageIndicator> DirectionDamageIndicatorWidget;
	TWeakObjectPtr<UGamePlayPortraitWidget> PortraitWidget;
};
