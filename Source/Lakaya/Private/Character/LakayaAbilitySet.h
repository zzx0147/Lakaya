// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

/**  */
UCLASS()
class LAKAYA_API ULakayaAbilitySet : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void GiveAbilities(class UAbilitySystemComponent* AbilitySystemComponent, const int32& InputOffset = 0);
	virtual void ClearAbilities();

	FORCEINLINE const TArray<struct FGameplayAbilitySpecHandle>& GetRegisteredAbilityHandles() const
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
