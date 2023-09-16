// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_Jump.h"

#include "Character/LakayaBaseCharacter.h"

ULakayaAbility_Jump::ULakayaAbility_Jump(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool ULakayaAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
	if (!LakayaBaseCharacter || !LakayaBaseCharacter->CanJump())
	{
		return false;
	}

	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void ULakayaAbility_Jump::NativeEndAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           bool bReplicateEndAbility, bool bWasCancelled)
{
	CharacterJumpStop();
	Super::NativeEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULakayaAbility_Jump::CharacterJumpStart()
{
	if (ALakayaBaseCharacter* LakayaBaseCharacter = Cast<ALakayaBaseCharacter>(GetAvatarActorFromActorInfo()))
	{
		if (LakayaBaseCharacter->IsLocallyControlled() && !LakayaBaseCharacter->bPressedJump)
		{
			LakayaBaseCharacter->UnCrouch();
			LakayaBaseCharacter->Jump();
		}
	}
}

void ULakayaAbility_Jump::CharacterJumpStop()
{
	if (ALakayaBaseCharacter* LakayaBaseCharacter = CurrentActorInfo ? Cast<ALakayaBaseCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr)
	{
		if (LakayaBaseCharacter->IsLocallyControlled() && LakayaBaseCharacter->bPressedJump)
		{
			LakayaBaseCharacter->StopJumping();
		}
	}
}
