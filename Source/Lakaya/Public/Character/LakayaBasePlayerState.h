// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Occupation/PlayerTeam.h"
#include "LakayaBasePlayerState.generated.h"

DECLARE_EVENT_OneParam(ALakayaBasePlayerState, FHealthChangeSignature, const float&)

DECLARE_EVENT_OneParam(ALakayaBasePlayerState, FTeamSignature, const EPlayerTeam&)

DECLARE_EVENT_ThreeParams(ALakayaBasePlayerState, FPlayerKillSignature, AController*, AActor*, AController*)

DECLARE_EVENT_OneParam(ALakayaBasePlayerState, FAliveChangeSignature, bool)

UCLASS()
class LAKAYA_API ALakayaBasePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void PreInitializeComponents() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

protected:
	virtual void CopyProperties(APlayerState* PlayerState) override;

public:
	// Other와 같은 팀인지 판별합니다.
	virtual bool IsSameTeam(const ALakayaBasePlayerState* Other) const;

	// 플레이어의 팀을 설정합니다.
	virtual void SetTeam(const EPlayerTeam& DesireTeam);

	/**
	 * @brief 플레이어가 예약된 시간에 부활하도록 합니다.
	 * @param ReservedRespawnTime 목표 부활 시간입니다. 이 시간에 플레이어가 부활합니다.
	 * 음수를 기입하여 기약없이 사망한 상태로, 0을 포함한 현재 시간보다 낮은 값을 기입하여 생존상태로 변경시킬 수도 있습니다.
	 * 이러한 경우 Object의 Function은 호출되지 않습니다.
	 * @param Object Function을 실행할 객체입니다.
	 * @param Function 부활 시간이 도래했을 때 실행할 Object의 멤버함수입니다.
	 */
	template <class T = nullptr_t>
	void SetRespawnTimer(const float& ReservedRespawnTime, T* Object = nullptr, void (T::*Function)() = nullptr);

	// 이 플레이어의 생존 여부를 가져옵니다.
	bool IsAlive() const;

	// 이 플레이어를 생존상태로 변경합니다.
	void MakeAlive();

protected:
	// 현재 서버의 시간을 가져옵니다.
	float GetServerTime() const;

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
	virtual void OnPawnSetCallback(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	// 최대 체력을 가져옵니다.
	virtual float GetMaxHealth() const;

	UFUNCTION()
	virtual void OnRep_Health();

	UFUNCTION()
	virtual void OnRep_Team();

	UFUNCTION()
	virtual void OnRep_RespawnTime();

private:
	// 생존 상태가 변경되었다면 이벤트를 호출하고, 그렇지 않다면 아무 것도 하지 않습니다.
	void BroadcastWhenAliveStateChanged();

public:
	// 현재 체력이 변경되는 경우 호출됩니다. 매개변수로 현재 체력을 받습니다.
	FHealthChangeSignature OnHealthChanged;

	// 현재 팀이 변경되는 경우 호출됩니다. 매개변수로 현재 팀을 받습니다.
	FTeamSignature OnTeamChanged;

	// 플레이어가 사망조건을 달성한 경우 호출됩니다. 매개변수로 살해당한 플레이어의 컨트롤러, 살해한 액터, 살해한 플레이어의 컨트롤러를 받습니다.
	FPlayerKillSignature OnPlayerKilled;

	// 플레이어가 살아나거나 죽는 경우 호출됩니다. 매개변수로 생존 상태를 받습니다. true이면 생존, false이면 사망을 의미합니다.
	FAliveChangeSignature OnAliveStateChanged;

private:
	UPROPERTY(ReplicatedUsing=OnRep_Health, Transient)
	float Health;

	UPROPERTY(ReplicatedUsing=OnRep_Team, Transient)
	EPlayerTeam Team;

	UPROPERTY(ReplicatedUsing=OnRep_RespawnTime, Transient)
	float RespawnTime;

	FTimerHandle RespawnTimer;
	bool bRecentAliveState;
};

template <class T>
void ALakayaBasePlayerState::SetRespawnTimer(const float& ReservedRespawnTime, T* Object, void (T::*Function)())
{
	RespawnTime = ReservedRespawnTime;
	BroadcastWhenAliveStateChanged();

	// 만약 RespawnTime이 현재 시간보다 낮게 설정된 경우 이 타이머는 설정되지 않습니다.
	GetWorldTimerManager().SetTimer(RespawnTimer, [this, Object, Function]
	{
		BroadcastWhenAliveStateChanged();
		if (Object && Function) (Object.*Function)();
	}, ReservedRespawnTime - GetServerTime(), false);
}
