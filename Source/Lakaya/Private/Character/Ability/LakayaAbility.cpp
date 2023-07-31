// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility.h"

#include "PlayerController/LakayaAbilityInputSet.h"

void ULakayaAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	if (!InputSet.IsNull() && ActorInfo->IsLocallyControlledPlayer())
	{
		InputSet.LoadSynchronous()->BindActions(GetEnhancedInputComponent(ActorInfo), this,
		                                        &ULakayaAbility::AbilityInput, InputHandleContainer);
	}
}

void ULakayaAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
	InputHandleContainer.RemoveBindings();
}

void ULakayaAbility::AbilityInput(TAbilitySystemInputCallback Function, int32 InputID)
{
	const auto AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (!ensure(AbilitySystemComponent)) return;
	(AbilitySystemComponent->*Function)(InputID);
}

UEnhancedInputComponent* ULakayaAbility::GetEnhancedInputComponent(const FGameplayAbilityActorInfo* ActorInfo)
{
	return ActorInfo && ActorInfo->PlayerController.IsValid()
		       ? Cast<UEnhancedInputComponent>(ActorInfo->PlayerController->InputComponent)
		       : nullptr;
}

ULocalPlayer* ULakayaAbility::GetLocalPlayer(const FGameplayAbilityActorInfo* ActorInfo)
{
	return ActorInfo && ActorInfo->PlayerController.IsValid()
		   ? ActorInfo->PlayerController->GetLocalPlayer()
		   : nullptr;
}

UEnhancedInputLocalPlayerSubsystem* ULakayaAbility::GetEnhancedInputSubsystem(
	const FGameplayAbilityActorInfo* ActorInfo)
{
	const auto LocalPlayer = GetLocalPlayer(ActorInfo);
	return LocalPlayer ? LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
}

UEnhancedInputLocalPlayerSubsystem* ULakayaAbility::GetCachedInputSubsystem(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (CachedInputSubsystem.IsValid()) return CachedInputSubsystem.Get();
	CachedInputSubsystem = GetEnhancedInputSubsystem(ActorInfo);
	return CachedInputSubsystem.Get();
}

void ULakayaAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!InputSet.IsNull())
	{
		if (const auto InputSubsystem = GetCachedInputSubsystem(ActorInfo))
		{
			InputSet.LoadSynchronous()->AddMappingContext(InputSubsystem);
		}
	}
}

void ULakayaAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (!InputSet.IsNull())
	{
		if (const auto InputSubsystem = GetCachedInputSubsystem(ActorInfo))
		{
			InputSet.LoadSynchronous()->RemoveMappingContext(InputSubsystem);
		}
	}
}
