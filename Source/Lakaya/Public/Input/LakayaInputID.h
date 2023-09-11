// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LakayaInputID.generated.h"

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

/** 입력 키와 오프셋값을 통해 어빌리티 시스템 컴포넌트에서 사용하는 InputID로의 변환을 구현합니다. */
USTRUCT(BlueprintType)
struct FLakayaInputID
{
	GENERATED_BODY()

	/** 어떤 키를 통해 어빌리티를 발동시킬지 지정합니다. */
	UPROPERTY(EditAnywhere)
	ELakayaAbilityInputBind Input;

	/** 입력자체는 동일하지만, 다른 InputID를 제공하기 위해 사용됩니다. */
	UPROPERTY(EditAnywhere)
	int32 InputOffset;

	/** 어빌리티 시스템 컴포넌트에 바인딩하기 위한 InputID값으로 변환시킵니다. */
	FORCEINLINE int32 GetInputID() const
	{
		return static_cast<int32>(Input) + StaticEnum<ELakayaAbilityInputBind>()->NumEnums() * InputOffset;
	}
};
