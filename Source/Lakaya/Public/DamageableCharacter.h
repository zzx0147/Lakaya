// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FocusableCharacter.h"
#include "DamageableCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FKillCharacterSignature, AController*, KilledController, AActor*,
                                              KilledActor, AController*, EventInstigator, AActor*, Causer);

UCLASS()
class LAKAYA_API ADamageableCharacter : public AFocusableCharacter
{
	GENERATED_BODY()

public:
	ADamageableCharacter();

	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

	/**
	 * @brief 이 캐릭터를 처치합니다.
	 * @param EventInstigator 처치한 컨트롤러
	 * @param DamageCauser 처치한 액터
	 */
	virtual void KillCharacter(AController* EventInstigator, AActor* DamageCauser);

	inline virtual const float& GetMaximumHealth() const { return MaximumHealth; }
	inline virtual const float& GetHealth() const { return Health; }
	inline virtual const float& FullHealth() { return Health = MaximumHealth; }
	
protected:
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

private:
	UFUNCTION()
	void OnTakeAnyDamageCallback(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	                             AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnKillCharacterCallback(AController* KilledController, AActor* KilledActor, AController* EventInstigator,
	                             AActor* DamageCauser);

public:
	UPROPERTY(Replicated)
	FKillCharacterSignature OnKillCharacter;

private:
	UPROPERTY(ReplicatedUsing=OnRep_MaximumHealth)
	float MaximumHealth;

	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float Health;
};
