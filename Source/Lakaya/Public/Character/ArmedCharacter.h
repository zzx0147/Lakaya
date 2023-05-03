// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageableCharacter.h"
#include "ArmedCharacter.generated.h"

enum EAbilityKind
{
	// 캐릭터의 첫번째 능력입니다.
	Primary,

	// 캐릭터의 두번째 능력입니다.
	Secondary,

	// 캐릭터의 무기 기본 능력입니다.
	WeaponFire,

	// 캐릭터의 무기 보조 능력입니다.
	WeaponAbility,

	// 캐릭터의 무기 재장전 또는 스킬 능력입니다.
	WeaponReload,

	// 캐릭터의 주요 이동관련 스킬입니다.
	Dash,

	// EAbilityKind의 총 개수를 의미합니다. 이 값은 항상 최하단에 위치해야 하며, 캐릭터의 스킬을 특정할 때 사용되는 값은 아닙니다. 
	Count
};

DECLARE_EVENT_OneParam(AArmedCharacter, FAbilitiesSignature, const TArray<class UCharacterAbility*>&)

/**
 * 
 */
UCLASS()
class LAKAYA_API AArmedCharacter : public ADamageableCharacter
{
	GENERATED_BODY()

public:
	virtual ELifetimeCondition
	AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const override;

protected:
	virtual void KillCharacter(AController* EventInstigator, AActor* DamageCauser) override;

public:
	virtual void SetupCharacterServer(const FCharacterSetupData* Data) override;

	/**
	 * @brief 캐릭터의 능력을 사용합니다. (키 누름)
	 * @param Kind 사용할 능력을 특정합니다.
	 */
	void StartAbility(const EAbilityKind& Kind);

	/**
	 * @brief 캐릭터의 능력 사용을 중단합니다. (키 뗌)
	 * @param Kind 사용할 능력을 특정합니다.
	 */
	void StopAbility(const EAbilityKind& Kind);

	// 캐릭터의 능력들을 가져옵니다. 이렇게 가져온 배열은 EAbilityKind를 통해 특정할 수 있습니다.
	const TArray<UCharacterAbility*>& GetAbilities() const { return Abilities; }

protected:
	UFUNCTION()
	virtual void OnRep_Abilities();

public:
	// 능력이 변경되면 호출됩니다. 매개변수로 전체 능력들을 넘겨주기 때문에 정확히 어떤 능력이 변경된 것인지는 알 수 없습니다.
	FAbilitiesSignature OnAbilitiesChanged;

protected:
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Abilities)
	TArray<UCharacterAbility*> Abilities;
};
