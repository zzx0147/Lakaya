#include "Character/Ability/Component/LakayaAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"
#include "GameplayTagResponseTable.h"

void ULakayaAbilitySystemComponent::AddInputInhibitedDelegate(const int32& InputID,
                                                              const FInputInhibitedDelegate& PressedDelegate,
                                                              const FInputInhibitedDelegate& ReleasedDelegate)
{
	const auto Pair = InputInhibitedDelegates.Find(InputID);
	if (Pair && Pair->IsBound())
	{
		UE_LOG(LogTemp, Warning, TEXT("InputID %d is already bound."), InputID);
	}
	InputInhibitedDelegates.Emplace(InputID, {PressedDelegate, ReleasedDelegate});
}

void ULakayaAbilitySystemComponent::RemoveInputInhibitedDelegate(const int32& InputID)
{
	InputInhibitedDelegates.Remove(InputID);
}

void ULakayaAbilitySystemComponent::AbilityLocalInputPressed(int32 InputID)
{
	if (!InputInhibitedDelegates.FindOrAdd(InputID).PressedIfBound(InputID))
	{
		Super::AbilityLocalInputPressed(InputID);
	}
}

void ULakayaAbilitySystemComponent::AbilityLocalInputReleased(int32 InputID)
{
	if (!InputInhibitedDelegates.FindOrAdd(InputID).ReleasedIfBound(InputID))
	{
		Super::AbilityLocalInputReleased(InputID);
	}
}

void ULakayaAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	//TODO: 언리얼 5.3의 스크립트이므로, 5.3에서는 제거해야 합니다.
	check(AbilityActorInfo.IsValid());
	bool WasAbilityActorNull = (AbilityActorInfo->AvatarActor == nullptr);
	bool AvatarChanged = (InAvatarActor != AbilityActorInfo->AvatarActor);

	AbilityActorInfo->InitFromActor(InOwnerActor, InAvatarActor, this);

	SetOwnerActor(InOwnerActor);

	// caching the previous value of the actor so we can check against it but then setting the value to the new because it may get used
	const AActor* PrevAvatarActor = GetAvatarActor_Direct();
	SetAvatarActor_Direct(InAvatarActor);

	// if the avatar actor was null but won't be after this, we want to run the deferred gameplaycues that may not have run in NetDeltaSerialize
	// Conversely, if the ability actor was previously null, then the effects would not run in the NetDeltaSerialize. As such we want to run them now.
	if ((WasAbilityActorNull || PrevAvatarActor == nullptr) && InAvatarActor != nullptr)
	{
		HandleDeferredGameplayCues(&ActiveGameplayEffects);
	}

	if (AvatarChanged)
	{
		ABILITYLIST_SCOPE_LOCK();
		for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			if (Spec.Ability)
			{
				if (Spec.Ability->GetInstancingPolicy() == EGameplayAbilityInstancingPolicy::InstancedPerActor)
				{
					UGameplayAbility* AbilityInstance = Spec.GetPrimaryInstance();
					// If we don't have the ability instance, it was either already destroyed or will get called on creation
					if (AbilityInstance)
					{
						AbilityInstance->OnAvatarSet(AbilityActorInfo.Get(), Spec);
					}
				}
				else
				{
					Spec.Ability->OnAvatarSet(AbilityActorInfo.Get(), Spec);
				}
			}
		}
	}

	if (UGameplayTagReponseTable* TagTable = UAbilitySystemGlobals::Get().GetGameplayTagResponseTable())
	{
		TagTable->RegisterResponseForEvents(this);
	}

	LocalAnimMontageInfo = FGameplayAbilityLocalAnimMontage();
	if (IsOwnerActorAuthoritative())
	{
		SetRepAnimMontageInfo(FGameplayAbilityRepAnimMontage());
	}

	if (bPendingMontageRep)
	{
		OnRep_ReplicatedAnimMontage();
	}
}
