// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterBindableWidget.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UCharacterBindableWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief 이 위젯에 캐릭터를 바인딩합니다.
	 * @param Character 바인딩 대상 캐릭터입니다.
	 */
	virtual void BindCharacter(ACharacter* const& Character) { return; }

	/**
	 * @brief 이 위젯의 바인딩을 해제합니다. 이 함수는 위젯이 캐릭터를 전혀 모르는 구조인 경우 사용됩니다.
	 * @param Character 바인딩 해제 대상 캐릭터입니다.
	 */
	virtual void UnbindCharacter(ACharacter* const& Character) { return; }
};
