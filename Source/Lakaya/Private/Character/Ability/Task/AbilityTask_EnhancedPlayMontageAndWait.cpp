// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Task/AbilityTask_EnhancedPlayMontageAndWait.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "GameFramework/Character.h"

UAbilityTask_EnhancedPlayMontageAndWait* UAbilityTask_EnhancedPlayMontageAndWait::EnhancedPlayMontageAndWait(
	UGameplayAbility* OwningAbility, FName TaskInstanceName, UAnimMontage* MontageToPlay, float Rate,
	FName StartSection, bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale, float StartTimeSeconds)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	auto* MyObj = NewAbilityTask<ThisClass>(OwningAbility, TaskInstanceName);
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	MyObj->StartTimeSeconds = StartTimeSeconds;

	return MyObj;
}

void UAbilityTask_EnhancedPlayMontageAndWait::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	bool bPlayedMontage = false;

	if (const auto ASC = AbilitySystemComponent.Get())
	{
		if (const auto AnimInstance = Ability->GetCurrentActorInfo()->GetAnimInstance())
		{
			if (ASC->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection,
			                     StartTimeSeconds) > 0.f)
			{
				// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
				if (ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}

				InterruptedHandle = Ability->OnGameplayAbilityCancelled.AddUObject(
					this, &UAbilityTask_PlayMontageAndWait::OnMontageInterrupted);

				BlendingOutDelegate.BindUObject(this, &UAbilityTask_PlayMontageAndWait::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UAbilityTask_PlayMontageAndWait::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				const auto MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
				MontageInstanceID = MontageInstance->GetInstanceID();

				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ThisClass::OnNotifyBeginReceived);
				AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &ThisClass::OnNotifyEndReceived);

				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (Character && (Character->GetLocalRole() == ROLE_Authority ||
					(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() ==
						EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				bPlayedMontage = true;
			}
		}
		else
		{
			ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWait call to PlayMontage failed!"));
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UAbilityTask_PlayMontageAndWait called on invalid AbilitySystemComponent"));
	}

	if (!bPlayedMontage)
	{
		ABILITY_LOG(Warning, TEXT(
			            "UAbilityTask_PlayMontageAndWait called in Ability %s failed to play montage %s; Task Instance Name %s."
		            ), *Ability->GetName(), *GetNameSafe(MontageToPlay), *InstanceName.ToString());

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast();
		}
	}

	SetWaitingOnAvatar();
}

void UAbilityTask_EnhancedPlayMontageAndWait::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);

	if (Ability)
	{
		if (const auto ActorInfo = Ability->GetCurrentActorInfo())
		{
			if (const auto AnimInstance = ActorInfo->GetAnimInstance())
			{
				AnimInstance->OnPlayMontageNotifyBegin.RemoveAll(this);
				AnimInstance->OnPlayMontageNotifyEnd.RemoveAll(this);
			}
		}
	}
}

void UAbilityTask_EnhancedPlayMontageAndWait::OnNotifyBeginReceived(
	FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (ShouldBroadcastAnimNotifyDelegates(BranchingPointNotifyPayload))
	{
		OnNotifyBegin.Broadcast(NotifyName, BranchingPointNotifyPayload);
	}
}

void UAbilityTask_EnhancedPlayMontageAndWait::OnNotifyEndReceived(
	FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (ShouldBroadcastAnimNotifyDelegates(BranchingPointNotifyPayload))
	{
		OnNotifyEnd.Broadcast(NotifyName, BranchingPointNotifyPayload);
	}
}

bool UAbilityTask_EnhancedPlayMontageAndWait::ShouldBroadcastAnimNotifyDelegates(
	const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const
{
	return BranchingPointNotifyPayload.MontageInstanceID == MontageInstanceID && ShouldBroadcastAbilityTaskDelegates();
}
