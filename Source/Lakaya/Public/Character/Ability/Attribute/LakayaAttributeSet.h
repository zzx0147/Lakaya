// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "LakayaAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class LAKAYA_API ULakayaAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ULakayaAttributeSet, Health);
	float GetHealth() const;
	void SetHealth(float NewVal);
	GAMEPLAYATTRIBUTE_VALUE_INITTER(Health);

	// GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ULakayaAttributeSet, Ammo);
	// GAMEPLAYATTRIBUTE_VALUE_GETTER(Ammo);
	// GAMEPLAYATTRIBUTE_VALUE_SETTER(Ammo);
	// GAMEPLAYATTRIBUTE_VALUE_INITTER(Ammo);
	//
	// GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ULakayaAttributeSet, AttackPoint);
	// GAMEPLAYATTRIBUTE_VALUE_GETTER(AttackPoint);
	// GAMEPLAYATTRIBUTE_VALUE_SETTER(AttackPoint);
	// GAMEPLAYATTRIBUTE_VALUE_INITTER(AttackPoint);

public:
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly,ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly)
	// FGameplayAttributeData Ammo;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly)
	// FGameplayAttributeData AttackPoint;

protected:
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);


};
