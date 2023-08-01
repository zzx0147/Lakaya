// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility.h"

#include "AbilitySystemLog.h"
#include "Input/LakayaInputContext.h"
#include "PlayerController/LakayaAbilityInputSet.h"

void ULakayaAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (!InputSet.IsNull() && ActorInfo->IsLocallyControlledPlayer())
	{
		if (!ensureMsgf(InstancingPolicy == EGameplayAbilityInstancingPolicy::InstancedPerActor,
		                TEXT("[%s]어빌리티가 InputSet 기능을 사용하려면 반드시 인스턴싱 정책을 InstancedPerActor로 설정해야 합니다."), *GetName()))
		{
			return;
		}

		InputSet.LoadSynchronous()->BindActions(GetEnhancedInputComponent(ActorInfo), this,
		                                        &ULakayaAbility::AbilityInput, InputHandleContainer);
	}
}

void ULakayaAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
	InputHandleContainer.RemoveBindings();
}

void ULakayaAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo)
{
	Log(ActorInfo, TEXT("Input Pressed"));
}

void ULakayaAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo)
{
	Log(ActorInfo, TEXT("Input Released"));
}

void ULakayaAbility::AbilityInput(TAbilitySystemInputCallback Function, int32 InputID)
{
	const auto AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if (!ensure(AbilitySystemComponent)) return;
	(AbilitySystemComponent->*Function)(InputID);
}

void ULakayaAbility::NativeEndAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      bool bReplicateEndAbility, bool bWasCancelled)
{
	if (!InputContext.IsNull())
	{
		if (const auto InputSubsystem = GetCachedInputSubsystem(ActorInfo))
		{
			InputContext.LoadSynchronous()->RemoveMappingContext(InputSubsystem);
		}
	}
	Log(ActorInfo, TEXT("End Ability"));
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
	const auto LocalPlayer = GetLocalPlayer(ActorInfo);
	return LocalPlayer ? LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
}

UEnhancedInputLocalPlayerSubsystem* ULakayaAbility::GetCachedInputSubsystem(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (CachedInputSubsystem.IsValid()) return CachedInputSubsystem.Get();
	CachedInputSubsystem = GetEnhancedInputSubsystem(ActorInfo);
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

void ULakayaAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!InputContext.IsNull())
	{
		if (const auto InputSubsystem = GetCachedInputSubsystem(ActorInfo))
		{
			InputContext.LoadSynchronous()->AddMappingContext(InputSubsystem);
		}
	}
	Log(ActorInfo, TEXT("Activate Ability"));
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
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	NativeCancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
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
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	NativeEndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
