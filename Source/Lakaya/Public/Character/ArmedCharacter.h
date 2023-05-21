// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovableCharacter.h"
#include "ArmedCharacter.generated.h"

UENUM(BlueprintType)
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
	const static TArray<FName> AbilityComponentNames;

	explicit AArmedCharacter(const FObjectInitializer& ObjectInitializer);

	virtual ELifetimeCondition
	AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const override;

protected:
	virtual void SetAliveState_Implementation(bool IsAlive) override;

public:
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

	/**
	 * @brief 서버에게 능력 사용을 요청합니다.
	 * @param Kind 능력 사용이 요청된 능력의 종류입니다.
	 * @param Time 클라이언트가 능력 사용을 요청한 시간입니다.
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestStartAbility(const EAbilityKind& Kind, const float& Time);

	/**
	 * @brief 서버에게 능력 중단을 요청합니다.
	 * @param Kind 능력 중단이 요청된 능력의 종류입니다.
	 * @param Time 클라이언트가 능력 중단을 요청한 시간입니다.
	 */
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestStopAbility(const EAbilityKind& Kind, const float& Time);

	// 캐릭터의 능력들을 가져옵니다. 이렇게 가져온 배열은 EAbilityKind를 통해 특정할 수 있습니다.
	FORCEINLINE const TArray<class UCharacterAbility*>& GetAbilities() const { return Abilities; }

	UFUNCTION(BlueprintCallable)
	UCharacterAbility* FindAbility(const EAbilityKind& Kind) const;

	template <class T>
	FORCEINLINE T* FindAbility(const EAbilityKind& Kind) const;

protected:
	// 능력 사용을 시작할지 여부를 조사합니다.
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldStartAbility(EAbilityKind Kind);

	// 능력 사용을 중단할지 여부를 조사합니다.
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldStopAbility(EAbilityKind Kind);

	// 서버에서 능력 사용을 시작할지 여부를 조사합니다.
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldStartAbilityOnServer(EAbilityKind Kind);

	// 서버에서 능력 사용을 중단할지 여부를 조사합니다.
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldStopAbilityOnServer(EAbilityKind Kind);

	UPROPERTY(EditAnywhere, Replicated)
	TArray<UCharacterAbility*> Abilities;
};

template <class T>
T* AArmedCharacter::FindAbility(const EAbilityKind& Kind) const
{
	if (Abilities.IsValidIndex(Kind)) return Cast<T>(Abilities[Kind]);
	return nullptr;
}
