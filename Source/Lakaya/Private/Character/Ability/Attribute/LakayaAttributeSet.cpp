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

void ULakayaAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, MaxAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, CurrentAmmo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULakayaAttributeSet, AttackPoint, COND_None, REPNOTIFY_Always);
}

