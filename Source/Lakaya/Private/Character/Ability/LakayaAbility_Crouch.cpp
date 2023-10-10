// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_Crouch.h"

#include "Character/LakayaBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ULakayaAbility_Crouch::ULakayaAbility_Crouch(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool ULakayaAbility_Crouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayTagContainer* SourceTags,
                                             const FGameplayTagContainer* TargetTags,
                                             FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid())
	{
		return false;
	}

	const ALakayaBaseCharacter* LakayaBaseCharacter = Cast<ALakayaBaseCharacter>(ActorInfo->AvatarActor.Get());
	if (!LakayaBaseCharacter || !LakayaBaseCharacter->CanCrouch())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void ULakayaAbility_Crouch::NativeEndAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           bool bReplicateEndAbility, bool bWasCancelled)
{
	CharacterCrouchStop();
	Super::NativeEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULakayaAbility_Crouch::CharacterCrouchStart()
{
	if (ALakayaBaseCharacter* LakayaBaseCharacter = Cast<ALakayaBaseCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (LakayaBaseCharacter->IsLocallyControlled())
		{
			if(LakayaBaseCharacter->GetCharacterMovement()->IsFalling() == false)
			{
				LakayaBaseCharacter->Crouch();
			}
		}
	}
}

void ULakayaAbility_Crouch::CharacterCrouchStop()
{
	if (ALakayaBaseCharacter* LakayaBaseCharacter = CurrentActorInfo ? Cast<ALakayaBaseCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr)
	{
		if (LakayaBaseCharacter->IsLocallyControlled())
		{
			LakayaBaseCharacter->UnCrouch();
		}
	}
}
