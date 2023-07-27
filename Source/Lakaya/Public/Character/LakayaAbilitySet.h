// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Engine/DataAsset.h"
#include "LakayaAbilitySet.generated.h"

UENUM(BlueprintType)
enum class ELakayaAbilityInputBind
{
	Ability1 = 0 UMETA(DisplayName = "Q"),
	Ability2 UMETA(DisplayName = "E"),
	Ability3 UMETA(DisplayName = "RMB"),
	Ability4 UMETA(DisplayName = "LMB"),
	Ability5 UMETA(DisplayName = "R"),
	Ability6 UMETA(DisplayName = "Shift"),
	Ability7 UMETA(DisplayName = "Space"),
	Ability8 UMETA(DisplayName = "Ctrl"),
	Ability9 UMETA(DisplayName = "F"),
};

/** 캐릭터의 능력과 입력을 쌍으로 받는 컨테이너 구조체입니다. */
USTRUCT(BlueprintType)
struct FLakayaAbilityBindInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	ELakayaAbilityInputBind Input;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameplayAbility> Class;
};

/**
 * @brief 입력을 수신하는 어빌리티들에 대한 정보를 저장하는 컨테이너 클래스입니다.
 * 주어지는 어빌리티 시스템 컴포넌트에 능력을 등록하거나, 해제하는 기능을 구현합니다.
 */
UCLASS()
class LAKAYA_API ULakayaAbilitySet : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * @brief 어빌리티 셋에 존재하는 어빌리티들을 어빌리티 시스템에 등록합니다.
	 * @param AbilitySystemComponent 어빌리티를 등록할 대상 어빌리티 시스템입니다.
	 * @param InputOffset 어빌리티 입력에 대한 오프셋입니다. 기본값은 0입니다.
	 */
	virtual void GiveAbilities(class UAbilitySystemComponent* AbilitySystemComponent, const int32& InputOffset = 0);

	/**
	 * @brief 이 어빌리티 셋이 등록한 어빌리티들을 어빌리티 시스템에서 해제합니다.
	 */
	virtual void ClearAbilities();

	/**
	 * @brief 이 어빌리티 셋이 등록한 어빌리티에 대한 핸들들을 반환합니다.
	 */
	FORCEINLINE const TArray<FGameplayAbilitySpecHandle>& GetRegisteredAbilityHandles() const
	{
		return RegisteredAbilities;
	}

protected:
	UPROPERTY(EditAnywhere)
	TArray<FLakayaAbilityBindInfo> Abilities;

private:
	TWeakObjectPtr<UAbilitySystemComponent> RegisteredAbilitySystem;
	TArray<FGameplayAbilitySpecHandle> RegisteredAbilities;
};
