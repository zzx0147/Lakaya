// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilityComponent.h"
#include "AbilityComponent_FireTrace.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UAbilityComponent_FireTrace : public UAbilityComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void FireTrace(TArray<FHitResult>& OutHitResults);
};
