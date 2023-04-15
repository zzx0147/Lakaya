// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FocusableCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "DrawDebugHelpers.h"
#include "DamageableCharacter.generated.h"

DECLARE_EVENT_FourParams(ADamageableCharacter, FKillCharacterSignature, AController*, AActor*, AController*, AActor*);

DECLARE_EVENT_OneParam(ADamageableCharacter, FRespawnSignature, AActor*);

DECLARE_EVENT_TwoParams(ADamageableCharacter, FMaximumHealthSignature, AActor*, const float&);

DECLARE_EVENT_TwoParams(ADamageableCharacter, FHealthSignature, AActor*, const float&);

UCLASS()
class LAKAYA_API ADamageableCharacter : public AFocusableCharacter
{
	GENERATED_BODY()

public:
	ADamageableCharacter(const FObjectInitializer& ObjectInitializer);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

public:
	inline virtual const float& GetMaximumHealth() const { return MaximumHealth; }
	inline virtual const float& GetHealth() const { return Health; }
	
	/**
	 * @brief 캐릭터가 리스폰 시 수행되어야 하는 기능을 실행합니다. 이 함수는 서버측에서만 실행됩니다.
	 */
	virtual void Respawn();

protected:
	/**
	 * @brief 이 캐릭터를 처치합니다. 이 함수는 항상 서버측에서만 호출됩니다.
	 * @param EventInstigator 처치한 컨트롤러
	 * @param DamageCauser 처치한 액터
	 */
	virtual void KillCharacter(AController* EventInstigator, AActor* DamageCauser);
	virtual void KillCharacterNotify_Implementation(AController* EventInstigator, AActor* DamageCauser);
	virtual void RespawnNotify_Implementation();

	/**
	 * @brief 최대 체력이 리플리케이트된 후 호출되는 이벤트 함수입니다. 서버측에서는 호출되지 않습니다.
	 */
	UFUNCTION()
	virtual void OnRep_MaximumHealth();

	/**
	 * @brief 현재 체력이 리플리케이트된 후 호출되는 이벤트 함수입니다. 서버측에서는 호출되지 않습니다.
	 */
	UFUNCTION()
	virtual void OnRep_Health();

	UFUNCTION()
	virtual void OnTakeAnyDamageCallback(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	                                     AController* InstigatedBy, AActor* DamageCauser);

private:
	UFUNCTION(NetMulticast, Reliable)
	void KillCharacterNotify(AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION(NetMulticast, Reliable)
	void RespawnNotify();

	UFUNCTION(NetMulticast, Reliable)
	void OnHitEffectPlay(AActor* DamagedActor);

	UFUNCTION(Client, Reliable)
	void IndicateRPC(FName CauserName, FVector DamageCursorPosition, float time);
	void IndicateRPC_Implementation(FName CauserName, FVector DamageCursorPosition, float time);

public:
	// 캐릭터가 사망했을 때 호출됩니다. 앞의 컨트롤러와 액터는 사망한 캐릭터, 뒤의 컨트롤러와 액터는 죽인 캐릭터를 의미합니다.
	FKillCharacterSignature OnKillCharacterNotify;

	// 캐릭터가 부활할 때 호출되는 이벤트입니다.
	FRespawnSignature OnRespawnCharacterNotify;

	// 최대체력이 변경되면 호출되는 이벤트입니다.
	FMaximumHealthSignature OnMaximumHealthChanged;

	// 현재체력이 변경되면 호출되는 이벤트입니다.
	FHealthSignature OnHealthChanged;

private:
	UPROPERTY(ReplicatedUsing=OnRep_MaximumHealth)
	float MaximumHealth;

	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float Health;
};
