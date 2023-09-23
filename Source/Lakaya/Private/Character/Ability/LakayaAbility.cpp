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
	OnInputPressed();
}

void ULakayaAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo)
{
	Log(ActorInfo, TEXT("Input Released"));
	OnInputReleased();
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
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, Message);
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

void ULakayaAbility::TargetDataScope()
{
	const auto ActorInfo = GetCurrentActorInfo();
	const auto ActivationMode = GetCurrentActivationInfoRef().ActivationMode;

	if (ActivationMode == EGameplayAbilityActivationMode::Authority && !ActorInfo->IsLocallyControlled())
	{
		TargetDataDelegateHandle = GetTargetDataDelegate().AddUObject(this, &ThisClass::OnTargetDataReceived);

		auto& TimerManager = GetWorld()->GetTimerManager();
		if (ServerTargetDataTimeOut > 0)
		{
			/*
			 * 애니메이션 몽타주를 실행하고, 그 몽타주의 애님 노티파이에서 타겟데이터를 생성하여 서버로 전송하는 경우에 타임아웃 옵션이 유용합니다.
			 * 이러한 경우 서버는 몽타주를 실행하고 타겟데이터를 기다리는데, 최소한 애님 노티파이가 실행되고 타겟 데이터가 전송되는 시간만큼 기다려야 할 것입니다.
			 * 따라서 어느정도 넉넉하게 타임아웃 시간을 적용해두고, 너무 긴 시간동안 타겟 데이터가 전송되지 않는다면 어빌리티를 취소하도록 합니다.
			 */
			TimerManager.SetTimer(TargetDataTimerHandle, this, &ThisClass::K2_CancelAbility, ServerTargetDataTimeOut);
		}
		else
		{
			/*
			 * 대개의 경우 클라이언트에서 ActivateAbility와 동시에 타겟 데이터를 전송하는데,
			 * 이러한 경우 ActivateAbility 스코프가 종료된 이후 즉시 타겟 데이터 이벤트가 호출됩니다.
			 * 따라서 다음 프레임까지 타겟데이터가 도착하지 않았다면 뭔가 잘못됐다고 판단하고 CancelAbility를 호출합니다.
			 */
			TargetDataTimerHandle = TimerManager.SetTimerForNextTick(this, &ThisClass::K2_CancelAbility);
		}
	}
	else
	{
		FScopedPredictionWindow PredictionWindow((GetAbilitySystemComponentFromActorInfo_Checked()));

		const auto TargetDataHandle = MakeTargetData();

		if (ActivationMode == EGameplayAbilityActivationMode::Predicting)
		{
			ServerSetReplicatedTargetData(TargetDataHandle);
		}
	}
}

void ULakayaAbility::OnTargetDataReceived_Implementation(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
                                                         FGameplayTag GameplayTag)
{
	GetWorld()->GetTimerManager().ClearTimer(TargetDataTimerHandle);
}

FGameplayAbilityTargetDataHandle ULakayaAbility::MakeTargetData_Implementation()
{
	return {};
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
