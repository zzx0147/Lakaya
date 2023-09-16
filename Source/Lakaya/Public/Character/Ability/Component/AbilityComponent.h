// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "AbilityComponent.generated.h"


class UAbilitySystemComponent;
class UGameplayAbility;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void SetOwningAbility(UGameplayAbility* InOwningAbility);

protected:
	void OnGameplayTagEvent(FGameplayTag Tag, int32 NewCount);

	UFUNCTION(BlueprintNativeEvent)
	void OnEnabled();

	UFUNCTION(BlueprintNativeEvent)
	void OnDisabled();

private:
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer DisableGameplayTags;

	TWeakObjectPtr<UGameplayAbility> OwningAbility;
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	bool bIsDisabled;
};
