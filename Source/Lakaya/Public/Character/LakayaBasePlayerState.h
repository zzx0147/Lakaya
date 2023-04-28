// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Occupation/PlayerTeam.h"
#include "LakayaBasePlayerState.generated.h"

DECLARE_EVENT_OneParam(ALakayaBasePlayerState, FHealthChangeSignature, const float&)

DECLARE_EVENT_OneParam(ALakayaBasePlayerState, FTeamSignature, const EPlayerTeam&)

DECLARE_EVENT_ThreeParams(ALakayaBasePlayerState, FPlayerKillSignature, AController*, AActor*, AController*)

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

protected:
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

public:
	// 현재 체력이 변경되는 경우 호출됩니다. 매개변수로 현재 체력을 받습니다.
	FHealthChangeSignature OnHealthChanged;

	// 현재 팀이 변경되는 경우 호출됩니다. 매개변수로 현재 팀을 받습니다.
	FTeamSignature OnTeamChanged;

	// 캐릭터가 사망하는 경우 호출됩니다. 매개변수로 살해당한 플레이어의 컨트롤러, 살해한 액터, 살해한 플레이어의 컨트롤러를 받습니다.
	FPlayerKillSignature OnPlayerKilled;

private:
	UPROPERTY(ReplicatedUsing=OnRep_Health, Transient)
	float Health;

	UPROPERTY(ReplicatedUsing=OnRep_Team, Transient)
	EPlayerTeam Team;
};
