// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/CustomImmunity/LakayaAbilityCustomImmunity.h"

bool ULakayaAbilityCustomImmunity::ShouldImmune_Implementation(const FGameplayEffectSpec& Spec,
                                                               const ULakayaAbilitySystemComponent* TargetASC) const
{
	return false;
}
