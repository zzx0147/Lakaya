// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_Immunity.h"

#include "AbilitySystemComponent.h"

void ULakayaAbility_Immunity::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
}

void ULakayaAbility_Immunity::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();

	ImmunityDelegateHandle = AbilitySystemComponent->OnImmunityBlockGameplayEffectDelegate.AddUObject(
		this, &ULakayaAbility_Immunity::OnImmunityBlockGameplayEffect);
}

void ULakayaAbility_Immunity::NativeEndAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::NativeEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);


	if (ImmunityDelegateHandle.IsValid())
	{
		UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
		AbilitySystemComponent->OnImmunityBlockGameplayEffectDelegate.Remove(ImmunityDelegateHandle);
	}
}

void ULakayaAbility_Immunity::OnImmunityBlockGameplayEffect(const FGameplayEffectSpec& BlockedSpec,
                                                            const FActiveGameplayEffect* ImmunityGameplayEffect)
{
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Parrying Something!"));

	for (const auto GamePlayEffectClass : ImmunityEffectArray)
	{
		if (ImmunityGameplayEffect->Spec.Def.IsA(GamePlayEffectClass))
		{
			K2_OnImmunityBlockGameplay(BlockedSpec, *ImmunityGameplayEffect);
			
			return;
		}
	}
}

float ULakayaAbility_Immunity::GetSetByCallerMagnitude(const FGameplayEffectSpec& Spec ,FGameplayTag DataTag, bool WarnIfNotFound,
	float DefaultIfNotFound)
{
	return Spec.GetSetByCallerMagnitude(DataTag,WarnIfNotFound,DefaultIfNotFound);
}
