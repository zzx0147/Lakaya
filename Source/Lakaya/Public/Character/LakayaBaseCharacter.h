// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Occupation/PlayerTeam.h"
#include "LakayaBaseCharacter.generated.h"

UCLASS()
class LAKAYA_API ALakayaBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ALakayaBaseCharacter();

	virtual ELifetimeCondition
	AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

protected:
	virtual float InternalTakeRadialDamage(float Damage, FRadialDamageEvent const& RadialDamageEvent,
	                                       AController* EventInstigator, AActor* DamageCauser) override;

public:
	// 캐릭터에 부착된 스프링암 컴포넌트를 가져옵니다.
	UFUNCTION(BlueprintGetter)
	class USpringArmComponent* const& GetSpringArm() const { return SpringArm; }

	// 캐릭터에 부착된 카메라 컴포넌트를 가져옵니다.
	UFUNCTION(BlueprintGetter)
	class UCameraComponent* const& GetCamera() const { return Camera; }

	// 캐릭터의 팀이 설정되는 경우 호출됩니다.
	UFUNCTION(BlueprintImplementableEvent)
	void OnSetTeam(const EPlayerTeam& Team);

	// 캐릭터의 자원 컴포넌트를 가져옵니다.
	UFUNCTION(BlueprintGetter)
	class UResourceComponent* const& GetResourceComponent() const { return ResourceComponent; }

	// 캐릭터의 자원 컴포넌트를 캐스팅하여 가져옵니다.
	template <class T>
	T* GetResourceComponent() const { return Cast<T>(ResourceComponent); }

	// 캐릭터 고유의 최대 체력을 가져옵니다. 플레이어의 최종적인 체력을 의미하지는 않습니다.
	UFUNCTION(BlueprintGetter)
	const float& GetCharacterMaxHealth() const { return MaxHealth; }

protected:
	// 현재 시점의 서버 시간을 가져옵니다.
	float GetServerTime() const;
	
	// 이 캐릭터의 고유한 최대 체력을 나타냅니다.
	UPROPERTY(EditAnywhere)
	float MaxHealth;

	UPROPERTY(EditAnywhere, Replicated)
	UResourceComponent* ResourceComponent;

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;
};
