// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "LakayaAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class LAKAYA_API ULakayaAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, MaxAmmo);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, CurrentAmmo);
	ATTRIBUTE_ACCESSORS(ULakayaAttributeSet, AttackPoint);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
};
