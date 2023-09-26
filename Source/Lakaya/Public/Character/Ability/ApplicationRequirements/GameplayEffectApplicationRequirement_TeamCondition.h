// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectCustomApplicationRequirement.h"
#include "GameplayEffectApplicationRequirement_TeamCondition.generated.h"

/**
 * 게임플레이 이펙트를 적용하는 플레이어와 타겟 플레이어의 팀 조건을 검사하도록 합니다.
 */
UCLASS()
class LAKAYA_API UGameplayEffectApplicationRequirement_TeamCondition
	: public UGameplayEffectCustomApplicationRequirement
{
	GENERATED_BODY()

	virtual bool CanApplyGameplayEffect_Implementation(const UGameplayEffect* GameplayEffect,
	                                                   const FGameplayEffectSpec& Spec,
	                                                   UAbilitySystemComponent* ASC) const override;
};
