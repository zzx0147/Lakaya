// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LakayaBlueprintLibrary.generated.h"

struct FGameplayAbilityTargetDataHandle;
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

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static void GetChildActors(const AActor* ParentActor, TArray<AActor*>& OutChildren);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UObject* GetClassDefaultObject(TSubclassOf<UObject> Class);

	UFUNCTION(BlueprintCallable, Category = "Online|Session", meta = (DisplayName = "ClientTravel"))
	static void K2_ClientTravel(APlayerController* Controller, FString ConnectString);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Online|Session")
	static int32 GetCurrentSessionPlayerCount();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FGameplayAbilityActorInfo GetActorInfoFromAbilitySystemComponent(
		UAbilitySystemComponent* AbilitySystemComponent);

	UFUNCTION(BlueprintCallable)
	static bool GetAttributeModifierMagnitude(const FGameplayEffectSpec& Spec, FGameplayAttribute Attribute,
	                                          float& OutMagnitude);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool HasAbility(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayAbility> AbilityClass);

	UFUNCTION(BlueprintCallable)
	static FGameplayAbilityTargetDataHandle MakeTargetDataFromHitResults(const TArray<FHitResult>& HitResults);

	UFUNCTION(BlueprintCallable)
	static void AddTargetDataFromHitResults(FGameplayAbilityTargetDataHandle& OutTargetDataHandle,
	                                        const TArray<FHitResult>& HitResults);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsGame();
};
