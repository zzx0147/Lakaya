// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/Attribute/LakayaAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

#define MARK_ATTRIBUTE_DIRTY_IF_CHANGED(AttributeName)\
	if (Attribute == Get##AttributeName##Attribute())\
	{\
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, AttributeName, this)\
	}


bool ULakayaAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return Super::PreGameplayEffectExecute(Data);
}

void ULakayaAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		if (OnPlayerKill.IsBound())
		{
			const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
			AActor* Instigator = EffectContext.GetOriginalInstigator();
			AActor* Causer = EffectContext.GetEffectCauser();

			const auto VictimPlayerState = Cast<APlayerState>(GetOwningActor());
			auto InstigatorPlayerState = Cast<APlayerState>(Instigator);

			if(!InstigatorPlayerState) //인스티게이터가 플레이어 스테이트가 아닌 경우(드론이 죽였을 경우)
			{
				auto InstigatorPlayerPawn = Instigator->GetInstigator();

				if(InstigatorPlayerPawn)
				{
					InstigatorPlayerState = Cast<APlayerState>(InstigatorPlayerPawn->GetPlayerState());
				}
			}
			
			if (VictimPlayerState != nullptr && InstigatorPlayerState != nullptr)
			{
				OnPlayerKill.Broadcast(VictimPlayerState->GetOwningController(),
				                       InstigatorPlayerState->GetOwningController(), Causer);
			}
			//피해자 컨트롤러 가해자 컨트롤러 가해자 액터
		}
	}

	bOutOfHealth = (GetHealth() <= 0.0f);
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

	MARK_ATTRIBUTE_DIRTY_IF_CHANGED(Health)
	MARK_ATTRIBUTE_DIRTY_IF_CHANGED(SkillStack)
	MARK_ATTRIBUTE_DIRTY_IF_CHANGED(CurrentAmmo)
	MARK_ATTRIBUTE_DIRTY_IF_CHANGED(UltimateGauge)

	if (Attribute == GetHealthAttribute())
	{
		if (OldValue > NewValue)
		{
			SetUltimateGauge(
				UltimateGauge.GetBaseValue() + (OldValue - NewValue) * UltimateGainOnAttacked.GetCurrentValue());
		}
	}

	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}
}

void ULakayaAttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue,
                                                  float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);
	MARK_ATTRIBUTE_DIRTY_IF_CHANGED(Health)
	MARK_ATTRIBUTE_DIRTY_IF_CHANGED(SkillStack)
	MARK_ATTRIBUTE_DIRTY_IF_CHANGED(CurrentAmmo)
	MARK_ATTRIBUTE_DIRTY_IF_CHANGED(UltimateGauge)
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
	UpdateMaxTagOnReplicated(GetSkillStackAttribute());
}

void ULakayaAttributeSet::OnRep_MaxSkillStack(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, MaxSkillStack, OldValue);
	UpdateMaxTagOnReplicated(GetMaxSkillStackAttribute());
}

void ULakayaAttributeSet::OnRep_EnergyHaste(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, EnergyHaste, OldValue);
}

void ULakayaAttributeSet::OnRep_Preparation(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, Preparation, OldValue);
}

void ULakayaAttributeSet::OnRep_Agility(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, Agility, OldValue);
}

void ULakayaAttributeSet::OnRep_UltimateGauge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, UltimateGauge, OldValue);
	UpdateMaxTagOnReplicated(GetUltimateGaugeAttribute());
}

void ULakayaAttributeSet::OnRep_MaxUltimateGauge(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, MaxUltimateGauge, OldValue);
	UpdateMaxTagOnReplicated(GetMaxUltimateGaugeAttribute());
}

void ULakayaAttributeSet::OnRep_UltimateGainOnAttack(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, UltimateGainOnAttack, OldValue);
}

void ULakayaAttributeSet::OnRep_UltimateGainOnAttacked(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, UltimateGainOnAttacked, OldValue);
}

void ULakayaAttributeSet::OnRep_UltimateGainOnSecond(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULakayaAttributeSet, UltimateGainOnSecond, OldValue);
}

ULakayaAttributeSet::ULakayaAttributeSet() : MaxHealth(100.0f), Health(100.0f), MaxAmmo(-1.0f), CurrentAmmo(-1.0f),
                                             AttackPoint(40.0f), SkillStack(3.0f), MaxSkillStack(3.0f),
                                             EnergyHaste(0.0f), Agility(0.0f), Preparation(0.0f),
                                             UltimateGauge(0.0f), MaxUltimateGauge(-1.0f), UltimateGainOnAttack(1.0f),
                                             UltimateGainOnAttacked(1.0f), UltimateGainOnSecond(0.0f)
{
	bOutOfHealth = false;

	RelatedAttributes = {
		{GetHealthAttribute(), GetMaxHealthAttribute()},
		{GetCurrentAmmoAttribute(), GetMaxAmmoAttribute()},
		{GetSkillStackAttribute(), GetMaxSkillStackAttribute(), &MaxSkillStackTag},
		{GetUltimateGaugeAttribute(), GetMaxUltimateGaugeAttribute(), &MaxUltimateGaugeTag}
	};
	SetupRelatedAttributes();
}

void ULakayaAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams Params;
	Params.RepNotifyCondition = REPNOTIFY_Always;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, Health, Params);
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, SkillStack, Params);
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, CurrentAmmo, Params);
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, UltimateGauge, Params);

	Params.bIsPushBased = false;
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, MaxHealth, Params);
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, MaxAmmo, Params);
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, AttackPoint, Params);
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, MaxSkillStack, Params);
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, EnergyHaste, Params);
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, Agility, Params);
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, Preparation, Params);
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, MaxUltimateGauge, Params);
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, UltimateGainOnAttack, Params);
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, UltimateGainOnAttacked, Params);
	DOREPLIFETIME_WITH_PARAMS(ULakayaAttributeSet, UltimateGainOnSecond, Params);
}
