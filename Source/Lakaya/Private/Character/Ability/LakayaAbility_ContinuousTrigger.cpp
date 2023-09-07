// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/LakayaAbility_ContinuousTrigger.h"

#include "AbilitySystemComponent.h"
#include "Character/Ability/Task/AbilityTask_WaitAbilityCooldown.h"
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
	const auto AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo_Checked();
	if (ActorInfo->IsNetAuthority())
	{
		TriggerAbilityHandle = AbilitySystemComponent->K2_GiveAbility(TriggerAbilityClass);
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

void ULakayaAbility_ContinuousTrigger::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* ActorInfo,
                                                       const FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* TriggerEventData)
{
	// 트리거 대상 어빌리티의 쿨다운을 기다리는 태스크를 실행합니다.
	const auto CooldownTask = UAbilityTask_WaitAbilityCooldown::WaitAbilityCooldown(this, TriggerAbilityHandle);
	CooldownTask->OnCooldownEnded.AddUObject(this, &ThisClass::TryActivateTriggerAbility);
	CooldownTask->ReadyForActivation();

	// 트리거 대상 어빌리티에 대해 활성화를 시도합니다.
	TryActivateTriggerAbility();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void ULakayaAbility_ContinuousTrigger::TryActivateTriggerAbility()
{
	if (!GetAbilitySystemComponentFromActorInfo_Checked()->TryActivateAbility(TriggerAbilityHandle))
	{
		Log(CurrentActorInfo, TEXT("Fail to Activate Trigger Ability. End this ability"));
		// 어빌리티 시전에 실패했다면 어빌리티를 더이상 활성화할 수 없는 것으로 간주하고 이 어빌리티도 종료시킵니다.
		K2_EndAbility();
	}
}
