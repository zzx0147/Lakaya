// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "AbilitySystemStats.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayCue_Types.h"
#include "ETC/LakayaPlayerCameraManager.h"
#include "Input/LakayaInputContext.h"

#define ENSURE_REMOTE_SERVER_SCOPE() \
	if (!ensure(IsForRemoteClient())) \
	{ \
		return; \
	}

void ULakayaAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo)
{
	Log(ActorInfo, TEXT("Input Pressed"));
	OnInputPressed();
}

void ULakayaAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo)
{
	Log(ActorInfo, TEXT("Input Released"));
	OnInputReleased();
}

void ULakayaAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	BP_OnGiveAbility(ActorInfo ? *ActorInfo : FGameplayAbilityActorInfo(), Spec);
}

void ULakayaAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	BP_OnAvatarSet(ActorInfo ? *ActorInfo : FGameplayAbilityActorInfo(), Spec);
}

void ULakayaAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
	BP_OnRemoveAbility(ActorInfo ? *ActorInfo : FGameplayAbilityActorInfo(), Spec);
}

void ULakayaAbility::NativeEndAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      bool bReplicateEndAbility, bool bWasCancelled)
{
	Log(ActorInfo, TEXT("End Ability"));
	if (TargetDataDelegateHandle.IsValid())
	{
		GetTargetDataDelegate().Remove(TargetDataDelegateHandle);
	}
}

void ULakayaAbility::NativeCancelAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         bool bReplicateCancelAbility)
{
	Log(ActorInfo, TEXT("Cancel Ability"));
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
	// 인스턴스를 생성하지 않는 경우 서브시스템을 캐싱하면 안되므로, 매번 새로 가져옵니다. 
	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		return InternalGetEnhancedInputSubsystem(ActorInfo);
	}

	// 서브시스템이 유효하지 않는 경우 업데이트합니다.
	if (!CachedInputSubsystem.IsValid())
	{
		CachedInputSubsystem = InternalGetEnhancedInputSubsystem(ActorInfo);
	}

	return CachedInputSubsystem.Get();
}

FString ULakayaAbility::LogFormat(const FGameplayAbilityActorInfo* ActorInfo, const FString& Message) const
{
	static const auto Format = TEXT("[{0}{1}][{2}] : {3}");
	static const auto ServerString = TEXT("Server");
	static const auto ClientString = TEXT("Client");
	static const auto ProbString = TEXT("?");

	const bool Authority = ActorInfo ? ActorInfo->IsNetAuthority() : GIsServer;

	return FString::Format(
		Format, {Authority ? ServerString : ClientString, ActorInfo ? TEXT("") : ProbString, GetName(), Message});
}

void ULakayaAbility::Log(const FString& Message) const
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
	if (!bAddLogOnScreen || !GEngine) return;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Message);
}

void ULakayaAbility::BP_Log(const FString& Message) const
{
	Log(CurrentActorInfo, Message);
}

void ULakayaAbility::ServerSetReplicatedTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
                                                   const FGameplayTag& GameplayTag) const
{
	const auto AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();
	AbilitySystemComponent->CallServerSetReplicatedTargetData(
		GetCurrentAbilitySpecHandle(), GetCurrentActivationInfo().GetActivationPredictionKey(), TargetDataHandle,
		GameplayTag, AbilitySystemComponent->ScopedPredictionKey);
}

FAbilityTargetDataSetDelegate& ULakayaAbility::GetTargetDataDelegate() const
{
	return GetAbilitySystemComponentFromActorInfo_Checked()->AbilityTargetDataSetDelegate(
		GetCurrentAbilitySpecHandle(), GetCurrentActivationInfoRef().GetActivationPredictionKey());
}

const FGameplayAbilityActivationInfo& ULakayaAbility::GetCurrentActivationInfoRef() const
{
	checkf(IsInstantiated(), TEXT(
		       "%s: GetCurrentActivationInfoRef cannot be called on a non-instanced ability. Check the instancing policy."
	       ), *GetPathName());
	return CurrentActivationInfo;
}

void ULakayaAbility::ConsumeTargetData() const
{
	GetAbilitySystemComponentFromActorInfo_Checked()->ConsumeClientReplicatedTargetData(
		GetCurrentAbilitySpecHandle(), GetCurrentActivationInfo().GetActivationPredictionKey());
}

TArray<FActiveGameplayEffectHandle> ULakayaAbility::ApplyGameplayEffectOrCueToTarget(
	const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayAbilityTargetDataHandle& Target,
	FGameplayTag GameplayCueTag, TSubclassOf<UGameplayEffect> GameplayEffectClass, float GameplayEffectLevel,
	int32 Stacks) const
{
	//bug: Undefined symbols:0> _StatPtr_STAT_ApplyGameplayEffectToTarget
	// SCOPE_CYCLE_COUNTER(STAT_ApplyGameplayEffectToTarget);

	SCOPE_CYCLE_UOBJECT(This, this);
	SCOPE_CYCLE_UOBJECT(Effect, GameplayEffectClass);

	TArray<FActiveGameplayEffectHandle> EffectHandles;

	if (HasAuthority(&ActivationInfo) == false && !UAbilitySystemGlobals::Get().ShouldPredictTargetGameplayEffects())
	{
		// Early out to avoid making effect specs that we can't apply
		return EffectHandles;
	}

	// This batches all created cues together
	FScopedGameplayCueSendContext GameplayCueSendContext;

	if (GameplayEffectClass == nullptr)
	{
		ABILITY_LOG(Error, TEXT("ApplyGameplayEffectToTarget called on ability %s with no GameplayEffect."),
		            *GetName());
	}
	else if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(
			Handle, ActorInfo, ActivationInfo, GameplayEffectClass, GameplayEffectLevel);

		if (SpecHandle.Data.IsValid())
		{
			SpecHandle.Data->StackCount = Stacks;

			SCOPE_CYCLE_UOBJECT(Source, SpecHandle.Data->GetContext().GetSourceObject());
			EffectHandles.Append(ApplyGameplayEffectSpecOrCueToTarget(
				Handle, ActorInfo, ActivationInfo, SpecHandle, Target, GameplayCueTag));
		}
		else
		{
			ABILITY_LOG(
				Warning,
				TEXT("UGameplayAbility::ApplyGameplayEffectToTarget failed to create valid spec handle. Ability: %s"),
				*GetPathName());
		}
	}

	return EffectHandles;
}

TArray<FActiveGameplayEffectHandle> ULakayaAbility::ApplyGameplayEffectSpecOrCueToTarget(
	const FGameplayAbilitySpecHandle AbilityHandle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEffectSpecHandle SpecHandle,
	const FGameplayAbilityTargetDataHandle& TargetData, FGameplayTag GameplayCueTag) const
{
	if (!SpecHandle.IsValid() || !HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		return {};
	}

	TArray<FActiveGameplayEffectHandle> EffectHandles;

	TARGETLIST_SCOPE_LOCK(*ActorInfo->AbilitySystemComponent);

	for (TSharedPtr<FGameplayAbilityTargetData> Data : TargetData.Data)
	{
		if (!Data.IsValid())
		{
			ABILITY_LOG(
				Warning,
				TEXT("UGameplayAbility::ApplyGameplayEffectSpecToTarget invalid target data passed in. Ability: %s"),
				*GetPathName());
			continue;
		}

		const auto AbilitySystemComponent = ActorInfo->AbilitySystemComponent;
		const auto PredictionKey = AbilitySystemComponent->GetPredictionKeyForNewAction();

		for (auto&& TargetActor : Data->GetActors())
		{
			FGameplayEffectSpec SpecToApply(*SpecHandle.Data);
			FGameplayEffectContextHandle EffectContext = SpecToApply.GetContext().Duplicate();
			SpecToApply.SetContext(EffectContext);

			Data->AddTargetDataToContext(EffectContext, false);

			if (const auto TargetComponent =
				UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor.Get()))
			{
				EffectHandles.Emplace(AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
					SpecToApply, TargetComponent, PredictionKey));
			}
			else
			{
				AbilitySystemComponent->ExecuteGameplayCue(GameplayCueTag, EffectContext);
			}
		}
	}

	return EffectHandles;
}

TArray<FActiveGameplayEffectHandle> ULakayaAbility::BP_ApplyGameplayEffectOrCueToTarget(
	FGameplayAbilityTargetDataHandle TargetData, TSubclassOf<UGameplayEffect> GameplayEffectClass,
	FGameplayTag GameplayCueTag, int32 GameplayEffectLevel, int32 Stacks)
{
	return ApplyGameplayEffectOrCueToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, TargetData,
	                                        GameplayCueTag, GameplayEffectClass, GameplayEffectLevel, Stacks);
}

TArray<FActiveGameplayEffectHandle> ULakayaAbility::K2_ApplyGameplayEffectSpecOrCueToTarget(
	const FGameplayEffectSpecHandle EffectSpecHandle, FGameplayTag GameplayCueTag,
	FGameplayAbilityTargetDataHandle TargetData)
{
	return ApplyGameplayEffectSpecOrCueToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
	                                            EffectSpecHandle, TargetData, GameplayCueTag);
}

void ULakayaAbility::InitiateInstantTargetDataScope()
{
	IsForRemoteClient() ? WaitForInstantTargetData() : CallMakeTargetData();
}

void ULakayaAbility::CallMakeTargetData()
{
	FScopedPredictionWindow PredictionWindow((GetAbilitySystemComponentFromActorInfo_Checked()));

	const auto TargetDataHandle = MakeTargetData();

	if (IsPredictingClient())
	{
		ServerSetReplicatedTargetData(TargetDataHandle);
	}
}

void ULakayaAbility::WaitForInstantTargetData()
{
	ENSURE_REMOTE_SERVER_SCOPE()
	BindTargetDataDelegate();
	TargetDataTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::K2_CancelAbility);
}

void ULakayaAbility::WaitForDelayedTargetData(const float& Delay)
{
	ENSURE_REMOTE_SERVER_SCOPE()
	BindTargetDataDelegate();
	GetWorld()->GetTimerManager().SetTimer(TargetDataTimerHandle, this, &ThisClass::K2_CancelAbility, Delay);
}

void ULakayaAbility::OnTargetDataReceived_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
                                                         FGameplayTag GameplayTag)
{
	GetWorld()->GetTimerManager().ClearTimer(TargetDataTimerHandle);
	ConsumeTargetData();
}

FGameplayAbilityTargetDataHandle ULakayaAbility::MakeTargetData_Implementation()
{
	return {};
}

void ULakayaAbility::SetZoom(const bool& bZoom, const float& ZoomFov, const FGameplayAbilityActorInfo* ActorInfo)
{
	if (ActorInfo && ActorInfo->PlayerController.IsValid())
	{
		if (const auto LakayaPlayerCameraManager = Cast<ALakayaPlayerCameraManager>(
			ActorInfo->PlayerController->PlayerCameraManager))
		{
			LakayaPlayerCameraManager->Zoom(bZoom, ZoomFov);
		}
	}
}

void ULakayaAbility::BP_SetZoom(const bool& bZoom, const float& ZoomFov)
{
	SetZoom(bZoom, ZoomFov, GetCurrentActorInfo());
}

void ULakayaAbility::AddMappingContext(const FGameplayAbilityActorInfo* ActorInfo,
                                       const ULakayaInputContext* InputContext)
{
	if (const auto InputSystem = GetEnhancedInputSubsystem(ActorInfo); InputSystem && InputContext)
	{
		InputContext->AddMappingContext(InputSystem);
	}
}

void ULakayaAbility::BP_AddMappingContext(const ULakayaInputContext* InputContext)
{
	AddMappingContext(GetCurrentActorInfo(), InputContext);
}

void ULakayaAbility::RemoveMappingContext(const FGameplayAbilityActorInfo* ActorInfo,
                                          const ULakayaInputContext* InputContext)
{
	if (const auto InputSystem = GetEnhancedInputSubsystem(ActorInfo); InputSystem && InputContext)
	{
		InputContext->RemoveMappingContext(InputSystem);
	}
}

void ULakayaAbility::BP_RemoveMappingContext(const ULakayaInputContext* InputContext)
{
	RemoveMappingContext(GetCurrentActorInfo(), InputContext);
}

void ULakayaAbility::HitResultsToTargetDataHandle(const TArray<FHitResult>& HitResults,
                                                  FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
	static auto Transform = [](const FHitResult& HitResult)
	{
		return new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
	};
	Algo::Transform(HitResults, OutTargetDataHandle, Transform);
}

void ULakayaAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Log(ActorInfo, TEXT("Activate Ability"));
	if (bHasBlueprintActivate)
	{
		// A Blueprinted ActivateAbility function must call CommitAbility somewhere in its execution chain.
		K2_ActivateAbility();
	}
	else if (bHasBlueprintActivateFromEvent)
	{
		if (TriggerEventData)
		{
			// A Blueprinted ActivateAbility function must call CommitAbility somewhere in its execution chain.
			K2_ActivateAbilityFromEvent(*TriggerEventData);
		}
		else
		{
			UE_LOG(LogAbilitySystem, Warning, TEXT(
				       "Ability %s expects event data but none is being supplied. Use Activate Ability instead of Activate Ability From Event."
			       ), *GetName());
			bool bReplicateEndAbility = false;
			bool bWasCancelled = true;
			EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		}
	}
	else
	{
		// Native child classes may want to override ActivateAbility and do something like this:

		// Do stuff...

		// if (CommitAbility(Handle, ActorInfo, ActivationInfo))		// ..then commit the ability...
		// 	{			
		// 	//	Then do more stuff...
		// 	}
	}
}

void ULakayaAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (!CanBeCanceled()) return;
	if (ScopeLockCount > 0)
	{
		UE_LOG(LogAbilitySystem, Verbose, TEXT(
			       "Attempting to cancel Ability %s but ScopeLockCount was greater than 0, adding cancel to the WaitingToExecute Array"
		       ), *GetName());
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ULakayaAbility::CancelAbility, Handle, ActorInfo,
		                                                      ActivationInfo, bReplicateCancelAbility));
		return;
	}
	NativeCancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void ULakayaAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
                                bool bWasCancelled)
{
	if (!IsEndAbilityValid(Handle, ActorInfo)) return;
	if (ScopeLockCount > 0)
	{
		UE_LOG(LogAbilitySystem, Verbose, TEXT(
			       "Attempting to end Ability %s but ScopeLockCount was greater than 0, adding end to the WaitingToExecute Array"
		       ), *GetName());
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ULakayaAbility::EndAbility, Handle, ActorInfo,
		                                                      ActivationInfo, bReplicateEndAbility, bWasCancelled));
		return;
	}
	NativeEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UEnhancedInputLocalPlayerSubsystem* ULakayaAbility::InternalGetEnhancedInputSubsystem(
	const FGameplayAbilityActorInfo* ActorInfo)
{
	const auto LocalPlayer = GetLocalPlayer(ActorInfo);
	return LocalPlayer ? LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
}

void ULakayaAbility::BindTargetDataDelegate()
{
	TargetDataDelegateHandle = GetTargetDataDelegate().AddUObject(this, &ThisClass::OnTargetDataReceived);
}
