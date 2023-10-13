// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "LakayaMagnitudeCalculation.generated.h"

/**
 * 상위 클래스의 함수들이 Const가 아니어서 블루프린트에서 사용할 수 없는 문제로, 부모 함수들을 Const로 만들어주는 클래스입니다
 */
UCLASS()
class LAKAYA_API ULakayaMagnitudeCalculation : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect")
	float GetSetByCallerMagnitudeByTagConst(const FGameplayEffectSpec& EffectSpec, const FGameplayTag& Tag) const;

	UFUNCTION(BlueprintCallable, Category = "Ability|GameplayEffect", meta=(DisplayName="Get Captured Attribute Magnitude Const", ScriptName="GetCapturedAttributeMagnitude"))
	float K2_GetCapturedAttributeMagnitudeConst(const FGameplayEffectSpec& EffectSpec, FGameplayAttribute Attribute, const FGameplayTagContainer& SourceTags, const FGameplayTagContainer& TargetTags) const;

	
};
