// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Attribute/LakayaAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"


bool ULakayaAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return Super::PreGameplayEffectExecute(Data);
}

void ULakayaAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// 이 게임플레이 이펙트는 Health를 변경합니다. 적용하되 우선 값을 제한합니다.
		// 이 경우 Health 베이스 값은 음수가 아니어야 합니다.
		SetHealth(FMath::Max(GetHealth(), 0.0f));
	}
}

void ULakayaAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void ULakayaAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void ULakayaAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetSkillStackAttribute())
	{
		if (FMath::IsNearlyEqual(MaxSkillStack.GetCurrentValue(), NewValue, 0.01f)) //대시 갯수가 꽉 참, 대쉬 갯수 리젠 이펙트를 꺼야 함
		{
			OnDashStackFullOrNot.Broadcast(true);
		}
		else if (FMath::IsNearlyEqual(MaxSkillStack.GetCurrentValue(), OldValue, 0.01f) && NewValue < OldValue)
		//최대치에서 감소함, 이때부터 대쉬 갯수 리젠 이펙트를 켜야 함
		{
			OnDashStackFullOrNot.Broadcast(false);
		}
	}
}

void ULakayaAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, MaxHealth, OldValue);
}

void ULakayaAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, Health, OldValue);
}

void ULakayaAttributeSet::OnRep_MaxAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, MaxAmmo, OldValue);
}

void ULakayaAttributeSet::OnRep_CurrentAmmo(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, CurrentAmmo, OldValue);
}

void ULakayaAttributeSet::OnRep_AttackPoint(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, AttackPoint, OldValue);
}

void ULakayaAttributeSet::OnRep_SkillStack(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, SkillStack, OldValue);
}

void ULakayaAttributeSet::OnRep_MaxSkillStack(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, MaxSkillStack, OldValue);
}

void ULakayaAttributeSet::OnRep_EnergyHaste(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, EnergyHaste, OldValue);
}

void ULakayaAttributeSet::OnRep_UltimateGauge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, UltimateGauge, OldValue);
}

ULakayaAttributeSet::ULakayaAttributeSet() : MaxHealth(100.0f), Health(100.0f), MaxAmmo(40.0f), CurrentAmmo(40.0f),
                                             AttackPoint(40.0f), SkillStack(3.0f), MaxSkillStack(3.0f)
{
}

void ULakayaAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, MaxAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, CurrentAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, AttackPoint, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, SkillStack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, MaxSkillStack, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, EnergyHaste, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, UltimateGauge, COND_None, REPNOTIFY_Always);
}
