// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAbility.h"
#include "StartRemoteCallAbility.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UStartRemoteCallAbility : public UCharacterAbility
{
	GENERATED_BODY()

public:
	virtual void AbilityStart() override;

protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void RequestStart(const float& RequestTime);
	virtual bool RequestStart_Validate(const float& RequestTime);
	virtual void RequestStart_Implementation(const float& RequestTime) { return; }

	// 시뮬레이트되는 서버 시간을 가져옵니다. 게임에서 기준 시간으로 사용됩니다.
	float GetServerTime() const;
};
