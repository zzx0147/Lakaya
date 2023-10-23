// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Components/ActorComponent.h"
#include "AbilityComponent.generated.h"


class UAbilitySystemComponent;
class UGameplayAbility;

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LAKAYA_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAbilityComponent();
	void SetOwningAbility(UGameplayAbility* InOwningAbility);

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UGameplayAbility* GetOwningAbility() const { return OwningAbility.Get(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent.Get(); }

	virtual bool ShouldActivate() const override;

private:
	void OnGameplayTagEvent(FGameplayTag Tag, int32 NewCount);

	UPROPERTY(EditAnywhere)
	FGameplayTagRequirements EnableRequireTags;

	TWeakObjectPtr<UGameplayAbility> OwningAbility;
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
