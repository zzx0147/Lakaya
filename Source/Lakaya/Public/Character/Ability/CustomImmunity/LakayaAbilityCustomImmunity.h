// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "UObject/Object.h"
#include "LakayaAbilityCustomImmunity.generated.h"

class ULakayaAbilitySystemComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class LAKAYA_API ULakayaAbilityCustomImmunity : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldImmune(const FGameplayEffectSpec& Spec, const ULakayaAbilitySystemComponent* TargetASC) const;
};
