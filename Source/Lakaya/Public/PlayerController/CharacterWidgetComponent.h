// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterWidgetComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UCharacterWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/**
	 * @brief 캐릭터 전용 위젯들을 셋업합니다.
	 * @param Character 위젯들을 바인딩할 대상 캐릭터입니다.
	 */
	virtual void SetupWidgetComponent(class AArmedCharacter* const& Character) { return; }

	/**
	 * @brief 캐릭터로부터 위젯들의 바인딩을 해제합니다.
	 * @param Character 바인딩 해제 대상 캐릭터입니다.
	 */
	virtual void UnbindCharacter(AArmedCharacter* const& Character) { return; }
};
