// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LakayaBlueprintLibrary.generated.h"

struct FGameplayEffectContextHandle;
struct FLakayaInputID;
/**
 * 
 */
UCLASS()
class LAKAYA_API ULakayaBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static int32 GetInputID(const FLakayaInputID& InputID);

	UFUNCTION(BlueprintCallable)
	static void SetSourceObject(FGameplayEffectContextHandle EffectContext, const UObject* SourceObject);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static ECollisionChannel ConvertToCollisionChannel(const EObjectTypeQuery& ObjectType);

	UFUNCTION(BlueprintCallable)
	static FGameplayEffectContextHandle GetEffectContext(const FGameplayEffectSpec& EffectSpec);
};
