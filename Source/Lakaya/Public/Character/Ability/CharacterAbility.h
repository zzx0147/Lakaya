// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAbility.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UCharacterAbility : public UActorComponent
{
	GENERATED_BODY()

public:
	// 능력 사용을 시작합니다. 이 함수는 클라이언트에서 호출됩니다.
	virtual void AbilityStart() { return; }

	// 능력 사용을 중단합니다. 이 함수는 클라이언트에서 호출됩니다.
	virtual void AbilityStop() { return; }

	// 캐릭터가 사망하는 경우 호출됩니다. 이 함수는 서버에서 호출됩니다.
	virtual void OnCharacterDead() { return; }
};
