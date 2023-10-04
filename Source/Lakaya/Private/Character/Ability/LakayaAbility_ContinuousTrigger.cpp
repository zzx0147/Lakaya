// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_ContinuousTrigger.h"

#include "AbilitySystemComponent.h"
#include "Character/Ability/Task/AbilityTask_WaitAbilityEnd.h"
#include "Character/Ability/Task/AbilityTask_WaitGameplayTagQueryWrapper.h"
#include "Net/UnrealNetwork.h"

ULakayaAbility_ContinuousTrigger::ULakayaAbility_ContinuousTrigger()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}

void ULakayaAbility_ContinuousTrigger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULakayaAbility_ContinuousTrigger, TriggerAbilityHandle);
}

void ULakayaAbility_ContinuousTrigger::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                                     const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (ActorInfo->IsNetAuthority())
	{
		TriggerAbilityHandle =
			GetAbilitySystemComponentFromActorInfo_Checked()->K2_GiveAbility(TriggerAbilityClass);
	}
}

void ULakayaAbility_ContinuousTrigger::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo,
                                                       const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
	if (ActorInfo->IsNetAuthority() && ActorInfo->AbilitySystemComponent.IsValid())
	{
		ActorInfo->AbilitySystemComponent->ClearAbility(TriggerAbilityHandle);
	}
}

void ULakayaAbility_ContinuousTrigger::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	if (!GetAbilitySystemComponentFromActorInfo_Checked()->FindAbilitySpecFromHandle(TriggerAbilityHandle)->IsActive())
	{
		K2_EndAbility();
	}
}

void ULakayaAbility_ContinuousTrigger::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* ActorInfo,
                                                       const FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* TriggerEventData)
{
	const auto AbilityEndTask = UAbilityTask_WaitAbilityEnd::WaitAbilityEnd(this, TriggerAbilityHandle);
	AbilityEndTask->OnAbilityEnded.AddUniqueDynamic(this, &ThisClass::OnTriggerAbilityEnded);
	AbilityEndTask->ReadyForActivation();

	const auto TriggerAbilitySpec =
		GetAbilitySystemComponentFromActorInfo_Checked()->FindAbilitySpecFromHandle(TriggerAbilityHandle);
	if (!TriggerAbilitySpec->IsActive())
	{
		WaitCooldown(TriggerAbilitySpec->Ability->GetCooldownTags());
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void ULakayaAbility_ContinuousTrigger::OnFailToActivateTriggerAbility_Implementation()
{
	K2_EndAbility();
}

void ULakayaAbility_ContinuousTrigger::OnTriggerAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	GetCurrentAbilitySpec()->InputPressed
		? WaitCooldown(AbilityEndedData.AbilityThatEnded->GetCooldownTags())
		: K2_EndAbility();
}

void ULakayaAbility_ContinuousTrigger::WaitCooldown(const FGameplayTagContainer* CooldownTags)
{
	// 쿨다운이 없다면 바로 트리거 어빌리티를 활성화합니다. 
	if (!CooldownTags)
	{
		TryActivateTriggerAbility();
		return;
	}

	// 쿨다운 종료를 기다립니다.
	const auto TagTask = UAbilityTask_WaitGameplayTagQueryWrapper::WaitGameplayTagQuery(
		this, FGameplayTagQuery::MakeQuery_MatchAnyTags(*CooldownTags), nullptr,
		EWaitGameplayTagQueryTriggerCondition::WhenFalse, true);
	TagTask->OnTriggered().AddDynamic(this, &ThisClass::TryActivateTriggerAbility);
	TagTask->ReadyForActivation();
}

void ULakayaAbility_ContinuousTrigger::TryActivateTriggerAbility()
{
	if (!GetAbilitySystemComponentFromActorInfo_Checked()->TryActivateAbility(TriggerAbilityHandle))
	{
		BP_Log(TEXT("Fail to Activate Trigger Ability"));
		OnFailToActivateTriggerAbility();
	}
}
