// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbility_Debug.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API UGameplayAbility_Debug : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGameplayAbility_Debug();
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                          const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo,
	                           bool bReplicateCancelAbility) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditAnywhere)
	uint8 bEndOnRelease : 1;

	FORCEINLINE static void AddDebugMessage(const FString& Message, const bool& bIsAuthority)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, bIsAuthority ? FColor::White : FColor::Red, Message);
	}

	FORCEINLINE void AddDebugMessage(const FString& Message, const FGameplayAbilityActivationInfo& ActivationInfo) const
	{
		AddDebugMessage(Message, HasAuthority(&ActivationInfo));
	}

	FORCEINLINE void AddDebugMessage(const FString& Message, const FGameplayAbilityActorInfo* ActorInfo) const
	{
		AddDebugMessage(Message, ActorInfo->IsNetAuthority());
	}
};
