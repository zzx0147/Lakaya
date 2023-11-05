#include "Character/Ability/Component/LakayaAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "GameplayCueManager.h"
#include "GameplayEffectCustomApplicationRequirement.h"
#include "GameplayTagResponseTable.h"
#include "Character/Ability/CustomImmunity/LakayaAbilityCustomImmunity.h"

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

FActiveGameplayEffectHandle ULakayaAbilitySystemComponent::ApplyGameplayEffectSpecToSelf(
	const FGameplayEffectSpec& Spec, FPredictionKey PredictionKey)
{
	// Scope lock the container after the addition has taken place to prevent the new effect from potentially getting mangled during the remainder
	// of the add operation
	FScopedActiveGameplayEffectLock ScopeLock(ActiveGameplayEffects);

	FScopeCurrentGameplayEffectBeingApplied ScopedGEApplication(&Spec, this);

	const bool bIsNetAuthority = IsOwnerActorAuthoritative();

	// Check Network Authority
	if (!HasNetworkAuthorityToApplyGameplayEffect(PredictionKey))
	{
		return FActiveGameplayEffectHandle();
	}

	// Don't allow prediction of periodic effects
	if (PredictionKey.IsValidKey() && Spec.GetPeriod() > 0.f)
	{
		if (IsOwnerActorAuthoritative())
		{
			// Server continue with invalid prediction key
			PredictionKey = FPredictionKey();
		}
		else
		{
			// Client just return now
			return FActiveGameplayEffectHandle();
		}
	}

	// Are we currently immune to this? (ApplicationImmunity)
	const FActiveGameplayEffect* ImmunityGE = nullptr;
	if (ActiveGameplayEffects.HasApplicationImmunityToSpec(Spec, ImmunityGE))
	{
		OnImmunityBlockGameplayEffect(Spec, ImmunityGE);
		return FActiveGameplayEffectHandle();
	}

	// Immune with CustomImmunities
	for (const auto& CustomImmunity : CustomImmunities)
	{
		if (CustomImmunity->ShouldImmune(Spec, this))
		{
			OnImmunedByCustomImmunity.Broadcast(Spec, CustomImmunity);
			return {};
		}
	}

	// Check AttributeSet requirements: make sure all attributes are valid
	// We may want to cache this off in some way to make the runtime check quicker.
	// We also need to handle things in the execution list
	for (const FGameplayModifierInfo& Mod : Spec.Def->Modifiers)
	{
		if (!Mod.Attribute.IsValid())
		{
			ABILITY_LOG(Warning, TEXT("%s has a null modifier attribute."), *Spec.Def->GetPathName());
			return FActiveGameplayEffectHandle();
		}
	}

	// check if the effect being applied actually succeeds
	float ChanceToApply = Spec.GetChanceToApplyToTarget();
	if ((ChanceToApply < 1.f - SMALL_NUMBER) && (FMath::FRand() > ChanceToApply))
	{
		return FActiveGameplayEffectHandle();
	}

	// Get MyTags.
	//	We may want to cache off a GameplayTagContainer instead of rebuilding it every time.
	//	But this will also be where we need to merge in context tags? (Headshot, executing ability, etc?)
	//	Or do we push these tags into (our copy of the spec)?

	{
		// Note: static is ok here since the scope is so limited, but wider usage of MyTags is not safe since this function can be recursively called
		static FGameplayTagContainer MyTags;
		MyTags.Reset();

		GetOwnedGameplayTags(MyTags);

		if (Spec.Def->ApplicationTagRequirements.RequirementsMet(MyTags) == false)
		{
			return FActiveGameplayEffectHandle();
		}

		if (!Spec.Def->RemovalTagRequirements.IsEmpty() && Spec.Def->RemovalTagRequirements.RequirementsMet(MyTags) ==
			true)
		{
			return FActiveGameplayEffectHandle();
		}
	}

	// Custom application requirement check
	for (const TSubclassOf<UGameplayEffectCustomApplicationRequirement>& AppReq : Spec.Def->ApplicationRequirements)
	{
		if (*AppReq && AppReq->GetDefaultObject<UGameplayEffectCustomApplicationRequirement>()->CanApplyGameplayEffect(
			Spec.Def, Spec, this) == false)
		{
			return FActiveGameplayEffectHandle();
		}
	}
	bIsNetDirty = true;

	// Clients should treat predicted instant effects as if they have infinite duration. The effects will be cleaned up later.
	bool bTreatAsInfiniteDuration = GetOwnerRole() != ROLE_Authority && PredictionKey.IsLocalClientKey() && Spec.Def->
		DurationPolicy == EGameplayEffectDurationType::Instant;

	// Make sure we create our copy of the spec in the right place
	// We initialize the FActiveGameplayEffectHandle here with INDEX_NONE to handle the case of instant GE
	// Initializing it like this will set the bPassedFiltersAndWasExecuted on the FActiveGameplayEffectHandle to true so we can know that we applied a GE
	FActiveGameplayEffectHandle MyHandle(INDEX_NONE);
	bool bInvokeGameplayCueApplied = Spec.Def->DurationPolicy != EGameplayEffectDurationType::Instant;
	// Cache this now before possibly modifying predictive instant effect to infinite duration effect.
	bool bFoundExistingStackableGE = false;

	FActiveGameplayEffect* AppliedEffect = nullptr;

	FGameplayEffectSpec* OurCopyOfSpec = nullptr;
	TSharedPtr<FGameplayEffectSpec> StackSpec;
	{
		if (Spec.Def->DurationPolicy != EGameplayEffectDurationType::Instant || bTreatAsInfiniteDuration)
		{
			AppliedEffect = ActiveGameplayEffects.ApplyGameplayEffectSpec(
				Spec, PredictionKey, bFoundExistingStackableGE);
			if (!AppliedEffect)
			{
				return FActiveGameplayEffectHandle();
			}

			MyHandle = AppliedEffect->Handle;
			OurCopyOfSpec = &(AppliedEffect->Spec);

			// Log results of applied GE spec
			if (UE_LOG_ACTIVE(VLogAbilitySystem, Log))
			{
				ABILITY_VLOG(GetOwnerActor(), Log, TEXT("Applied %s"), *OurCopyOfSpec->Def->GetFName().ToString());

				for (const FGameplayModifierInfo& Modifier : Spec.Def->Modifiers)
				{
					float Magnitude = 0.f;
					Modifier.ModifierMagnitude.AttemptCalculateMagnitude(Spec, Magnitude);
					ABILITY_VLOG(GetOwnerActor(), Log, TEXT("         %s: %s %f"), *Modifier.Attribute.GetName(),
					             *EGameplayModOpToString(Modifier.ModifierOp), Magnitude);
				}
			}
		}

		if (!OurCopyOfSpec)
		{
			StackSpec = TSharedPtr<FGameplayEffectSpec>(new FGameplayEffectSpec(Spec));
			OurCopyOfSpec = StackSpec.Get();
			UAbilitySystemGlobals::Get().GlobalPreGameplayEffectSpecApply(*OurCopyOfSpec, this);
			OurCopyOfSpec->CaptureAttributeDataFromTarget(this);
		}

		// if necessary add a modifier to OurCopyOfSpec to force it to have an infinite duration
		if (bTreatAsInfiniteDuration)
		{
			// This should just be a straight set of the duration float now
			OurCopyOfSpec->SetDuration(UGameplayEffect::INFINITE_DURATION, true);
		}
	}

	if (OurCopyOfSpec)
	{
		// Update (not push) the global spec being applied [we want to switch it to our copy, from the const input copy)
		UAbilitySystemGlobals::Get().SetCurrentAppliedGE(OurCopyOfSpec);
	}


	// We still probably want to apply tags and stuff even if instant?
	// If bSuppressStackingCues is set for this GameplayEffect, only add the GameplayCue if this is the first instance of the GameplayEffect
	if (!bSuppressGameplayCues && bInvokeGameplayCueApplied && AppliedEffect && !AppliedEffect->bIsInhibited &&
		(!bFoundExistingStackableGE || !Spec.Def->bSuppressStackingCues))
	{
		// We both added and activated the GameplayCue here.
		// On the client, which will invoke the gameplay cue from an OnRep, it will need to look at the StartTime to determine
		// if the Cue was actually added+activated or just added (due to relevancy)

		// Fixme: what if we wanted to scale Cue magnitude based on damage? E.g, scale an cue effect when the GE is buffed?

		if (OurCopyOfSpec->StackCount > Spec.StackCount)
		{
			// Because PostReplicatedChange will get called from modifying the stack count
			// (and not PostReplicatedAdd) we won't know which GE was modified.
			// So instead we need to explicitly RPC the client so it knows the GC needs updating
			UAbilitySystemGlobals::Get().GetGameplayCueManager()->InvokeGameplayCueAddedAndWhileActive_FromSpec(
				this, *OurCopyOfSpec, PredictionKey);
		}
		else
		{
			// Otherwise these will get replicated to the client when the GE gets added to the replicated array
			InvokeGameplayCueEvent(*OurCopyOfSpec, EGameplayCueEvent::OnActive);
			InvokeGameplayCueEvent(*OurCopyOfSpec, EGameplayCueEvent::WhileActive);
		}
	}

	// Execute the GE at least once (if instant, this will execute once and be done. If persistent, it was added to ActiveGameplayEffects above)

	// Execute if this is an instant application effect
	if (bTreatAsInfiniteDuration)
	{
		// This is an instant application but we are treating it as an infinite duration for prediction. We should still predict the execute GameplayCUE.
		// (in non predictive case, this will happen inside ::ExecuteGameplayEffect)

		if (!bSuppressGameplayCues)
		{
			UAbilitySystemGlobals::Get().GetGameplayCueManager()->InvokeGameplayCueExecuted_FromSpec(
				this, *OurCopyOfSpec, PredictionKey);
		}
	}
	else if (Spec.Def->DurationPolicy == EGameplayEffectDurationType::Instant)
	{
		if (OurCopyOfSpec->Def->OngoingTagRequirements.IsEmpty())
		{
			ExecuteGameplayEffect(*OurCopyOfSpec, PredictionKey);
		}
		else
		{
			ABILITY_LOG(
				Warning,
				TEXT(
					"%s is instant but has tag requirements. Tag requirements can only be used with gameplay effects that have a duration. This gameplay effect will be ignored."
				), *Spec.Def->GetPathName());
		}
	}

	if (Spec.GetPeriod() != UGameplayEffect::NO_PERIOD && Spec.TargetEffectSpecs.Num() > 0)
	{
		ABILITY_LOG(
			Warning,
			TEXT(
				"%s is periodic but also applies GameplayEffects to its target. GameplayEffects will only be applied once, not every period."
			), *Spec.Def->GetPathName());
	}

	// evaluate if any active effects need to be removed by the application of this effect
	if (bIsNetAuthority)
	{
		ActiveGameplayEffects.AttemptRemoveActiveEffectsOnEffectApplication(*OurCopyOfSpec, MyHandle);
	}

	// ------------------------------------------------------
	// Apply Linked effects
	// todo: this is ignoring the returned handles, should we put them into a TArray and return all of the handles?
	// ------------------------------------------------------
	for (const FGameplayEffectSpecHandle& TargetSpec : Spec.TargetEffectSpecs)
	{
		if (TargetSpec.IsValid())
		{
			ApplyGameplayEffectSpecToSelf(*TargetSpec.Data.Get(), PredictionKey);
		}
	}

	UAbilitySystemComponent* InstigatorASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();

	// Send ourselves a callback	
	OnGameplayEffectAppliedToSelf(InstigatorASC, *OurCopyOfSpec, MyHandle);

	// Send the instigator a callback
	if (InstigatorASC)
	{
		InstigatorASC->OnGameplayEffectAppliedToTarget(this, *OurCopyOfSpec, MyHandle);
	}

	return MyHandle;
}

void ULakayaAbilitySystemComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// 부모 함수에서 아바타 액터를 임의로 Owner로 설정해버리도록 되어있어 nullptr로 다시 바꿔줍니다.
	SetAvatarActor(nullptr);
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
