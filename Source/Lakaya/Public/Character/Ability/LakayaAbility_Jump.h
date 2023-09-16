// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Ability/LakayaAbility.h"
#include "LakayaAbility_Jump.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class LAKAYA_API ULakayaAbility_Jump : public ULakayaAbility
{
	GENERATED_BODY()

public:
	ULakayaAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void NativeEndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	UFUNCTION(BlueprintCallable, Category = "Lakaya|Ability")
	void CharacterJumpStart();

	UFUNCTION(BlueprintCallable, Category = "Lakaya|Ability")
	void CharacterJumpStop();

protected:
	mutable const FGameplayAbilityActorInfo* CurrentActorInfo;

	/** For instanced abilities */
	mutable FGameplayAbilitySpecHandle CurrentSpecHandle;
};
