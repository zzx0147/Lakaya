// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "LakayaBaseAttributeSet.h"
#include "LakayaAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDashStackFullOrNot, const bool, bIsFull);

DECLARE_EVENT_ThreeParams(ALakayaBasePlayerState, FPlayerKilledSignature, AController*, AController*, AActor*)

DECLARE_EVENT_OneParam(ALakayaBasePlayerState, FAttributeChangeSignature, const float&)

UCLASS(Config=Game)
class LAKAYA_API ULakayaAttributeSet : public ULakayaBaseAttributeSet
{
	GENERATED_BODY()

public:
	ULakayaAttributeSet();

	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, MaxAmmo);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, CurrentAmmo);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, AttackPoint);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, SkillStack);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, MaxSkillStack);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, EnergyHaste);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, UltimateGauge);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, MaxUltimateGauge);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, UltimateGainOnAttack);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, UltimateGainOnAttacked);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, UltimateGainOnSecond);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnDashStackFullOrNot OnDashStackFullOrNot;

	mutable FAttributeChangeSignature OnHealthChanged;

	mutable FAttributeChangeSignature OnMaxHealthChanged;

	mutable FPlayerKilledSignature OnPlayerKill;

protected:
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MaxAmmo)
	FGameplayAttributeData MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentAmmo)
	FGameplayAttributeData CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing= OnRep_AttackPoint)
	FGameplayAttributeData AttackPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing= OnRep_SkillStack)
	FGameplayAttributeData SkillStack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing= OnRep_MaxSkillStack)
	FGameplayAttributeData MaxSkillStack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing= OnRep_EnergyHaste)
	FGameplayAttributeData EnergyHaste;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing= OnRep_UltimateGauge)
	FGameplayAttributeData UltimateGauge;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing= OnRep_MaxUltimateGauge)
	FGameplayAttributeData MaxUltimateGauge;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing= OnRep_UltimateGainOnAttack)
	FGameplayAttributeData UltimateGainOnAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing= OnRep_UltimateGainOnAttacked)
	FGameplayAttributeData UltimateGainOnAttacked;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing= OnRep_UltimateGainOnSecond)
	FGameplayAttributeData UltimateGainOnSecond;

	
protected:
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_MaxAmmo(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_CurrentAmmo(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_AttackPoint(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_SkillStack(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_MaxSkillStack(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_EnergyHaste(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_UltimateGauge(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_MaxUltimateGauge(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_UltimateGainOnAttack(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_UltimateGainOnAttacked(const FGameplayAttributeData& OldValue);
	UFUNCTION()
	virtual void OnRep_UltimateGainOnSecond(const FGameplayAttributeData& OldValue);

private:
	UPROPERTY(GlobalConfig)
	FGameplayTag MaxSkillStackTag;

	UPROPERTY(GlobalConfig)
	FGameplayTag MaxUltimateGaugeTag;

	bool bOutOfHealth;
};
