// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovableCharacter.h"
#include "DamageableCharacter.generated.h"

DECLARE_EVENT_OneParam(ADamageableCharacter, FAliveSignature, bool);

DECLARE_EVENT_OneParam(ADamageableCharacter, FHealthSignature, const float&);

DECLARE_EVENT_ThreeParams(ADamageableCharacter, FDamageSignature, const FString&, const FVector&, const float&);

UCLASS()
class LAKAYA_API ADamageableCharacter : public AMovableCharacter
{
	GENERATED_BODY()

public:
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

protected:
	virtual void SetupCharacterServer(const FCharacterSetupData* Data) override;

public:
	// 캐릭터의 현재 체력을 가져옵니다.
	const float& GetHealth() const { return Health; }

	// 캐릭터가 현재 살아있는지 여부를 조사합니다.
	const bool& GetIsAlive() const { return bIsAlive; }

	// 캐릭터가 리스폰 시 수행되어야 하는 기능을 실행합니다. 이 함수는 반드시 서버에서만 실행되어야 합니다.
	virtual void Respawn();

protected:
	/**
	 * @brief 이 캐릭터를 처치합니다. 이 함수는 반드시 서버에서만 실행되어야 합니다.
	 * @param EventInstigator 처치한 컨트롤러
	 * @param DamageCauser 처치한 액터
	 */
	virtual void KillCharacter(AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	virtual void OnRep_Health();

	UFUNCTION()
	virtual void OnRep_IsAlive();

private:
	/**
	 * @brief 이 캐릭터를 소유한 인스턴스에게 이 캐릭터가 다른 캐릭터에 의해 피해를 받았음을 알립니다.
	 * @param CauserName 이 캐릭터를 공격한 액터의 이름입니다.
	 * @param AttackLocation 공격한 캐릭터가 이 캐릭터에 대한 공격을 시작한 위치입니다.
	 * @param Damage 이 캐릭터가 받은 피해량입니다.
	 */
	UFUNCTION(Client, Reliable)
	void NotifyDamageReceive(const FString& CauserName, const FVector& AttackLocation, const float& Damage);

public:
	// 캐릭터가 사망 또는 부활할 때 호출됩니다. 매개변수가 true이면 부활, 그렇지 않으면 사망을 의미합니다.
	FAliveSignature OnAliveChanged;

	// 현재체력이 변경되면 호출되는 이벤트입니다.
	FHealthSignature OnHealthChanged;

	// 캐릭터가 데미지를 수신하면 호출됩니다. 이 이벤트는 오너 클라이언트에서만 호출됩니다. 매개변수로 이 캐릭터를 공격한 캐릭터와 받은 피해량을 받습니다.
	FDamageSignature OnDamageReceived;

private:
	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float Health;

	UPROPERTY(ReplicatedUsing=OnRep_IsAlive)
	bool bIsAlive;
};
