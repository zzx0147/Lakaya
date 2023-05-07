// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovableCharacter.h"
#include "ArmedCharacter.generated.h"

UENUM()
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

/**
 * 
 */
UCLASS()
class LAKAYA_API AArmedCharacter : public AMovableCharacter
{
	GENERATED_BODY()

public:
	explicit AArmedCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual ELifetimeCondition
	AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const override;

	/**
	 * @brief 캐릭터의 능력을 사용합니다. (키 누름)
	 * @param Kind 사용할 능력을 특정합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void StartAbility(const EAbilityKind& Kind);

	/**
	 * @brief 캐릭터의 능력 사용을 중단합니다. (키 뗌)
	 * @param Kind 사용할 능력을 특정합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void StopAbility(const EAbilityKind& Kind);

	// 캐릭터의 능력들을 가져옵니다. 이렇게 가져온 배열은 EAbilityKind를 통해 특정할 수 있습니다.
	UFUNCTION(BlueprintGetter)
	const TArray<class UCharacterAbility*>& GetAbilities() const { return Abilities; }

protected:
	UPROPERTY(EditAnywhere, Replicated)
	TArray<UCharacterAbility*> Abilities;
};
