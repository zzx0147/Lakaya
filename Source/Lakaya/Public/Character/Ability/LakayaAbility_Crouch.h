// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Ability/LakayaAbility.h"
#include "LakayaAbility_Crouch.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class LAKAYA_API ULakayaAbility_Crouch : public ULakayaAbility
{
	GENERATED_BODY()
	
public:
	ULakayaAbility_Crouch(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void NativeEndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, Category = "Lakaya|Ability")
	void CharacterCrouchStart();

	UFUNCTION(BlueprintCallable, Category = "Lakaya|Ability")
	void CharacterCrouchStop();

protected:
	mutable const FGameplayAbilityActorInfo* CurrentActorInfo;

	/** For instanced abilities */
	mutable FGameplayAbilitySpecHandle CurrentSpecHandle;

};
