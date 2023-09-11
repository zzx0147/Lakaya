// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void ULakayaAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo)
{
	Log(ActorInfo, TEXT("Input Pressed"));
}

void ULakayaAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo)
{
	Log(ActorInfo, TEXT("Input Released"));
	if (bEndOnInputRelease)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void ULakayaAbility::NativeEndAbility(const FGameplayAbilitySpecHandle Handle,
                                      const FGameplayAbilityActorInfo* ActorInfo,
                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                      bool bReplicateEndAbility, bool bWasCancelled)
{
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
	// 인스턴스를 생성하지 않는 경우 서브시스템을 캐싱하면 안되므로, 매번 새로 가져옵니다. 
	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		return InternalGetEnhancedInputSubsystem(ActorInfo);
	}

	// 서브시스템이 유효하지 않는 경우 업데이트합니다.
	if (CachedInputSubsystem.IsValid())
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

void ULakayaAbility::ServerSetReplicatedTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
                                                   const FGameplayTag& GameplayTag) const
{
	const auto AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();
	AbilitySystemComponent->CallServerSetReplicatedTargetData(
		GetCurrentAbilitySpecHandle(), GetCurrentActivationInfo().GetActivationPredictionKey(), TargetDataHandle,
		GameplayTag, AbilitySystemComponent->ScopedPredictionKey);
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
			UE_LOG(LogAbilitySystem, Warning, TEXT("Ability %s expects event data but none is being supplied. Use Activate Ability instead of Activate Ability From Event."), *GetName());
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
