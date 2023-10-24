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
	UFUNCTION(BlueprintGetter)
	UGameplayAbility* GetOwningAbility() const { return OwningAbility; }

	UFUNCTION(BlueprintGetter)
	UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }

	virtual bool ShouldActivate() const override;

private:
	void OnRequiredTagUpdated(FGameplayTag Tag, int32 NewCount);
	void OnIgnoredTagUpdated(FGameplayTag Tag, int32 NewCount);

	UPROPERTY(EditAnywhere)
	FGameplayTagRequirements EnableRequireTags;

	UPROPERTY(Transient, BlueprintGetter=GetOwningAbility)
	TObjectPtr<UGameplayAbility> OwningAbility;

	UPROPERTY(Transient, BlueprintGetter=GetAbilitySystemComponent)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
