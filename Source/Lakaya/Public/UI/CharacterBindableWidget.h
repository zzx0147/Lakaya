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
	 * @param Character 바인딩 대상 캐릭터입니다. nullptr이 아님이 보장됩니다.
	 */
	virtual void BindCharacter(ACharacter* const& Character) { return; }

	/**
	 * @brief 이 위젯의 바인딩을 해제합니다. 이 함수는 위젯이 캐릭터를 전혀 모르는 구조인 경우 사용됩니다.
	 * @param Character 바인딩 해제 대상 캐릭터입니다. nullptr이 아님이 보장됩니다.
	 * @return 이 위젯이 바인딩이 해제된 뒤에도 남아있을지 여부입니다. true를 반환하면 남게 되며, false를 반환하면 제거됩니다.
	 */
	virtual bool UnbindCharacter(ACharacter* const& Character) { return false; }
};
