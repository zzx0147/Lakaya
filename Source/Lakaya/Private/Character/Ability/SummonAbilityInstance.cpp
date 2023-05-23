// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/SummonAbilityInstance.h"

#include "Character/Ability/CoolTimedSummonAbility.h"
#include "Net/UnrealNetwork.h"

ASummonAbilityInstance::ASummonAbilityInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
}

void ASummonAbilityInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASummonAbilityInstance, OwningAbility)
}


void ASummonAbilityInstance::OnSummoned()
{
}

void ASummonAbilityInstance::NotifyAbilitySummoned()
{
	if (OwningAbility) OwningAbility->NotifyAbilityInstanceSummoned(this);
}

void ASummonAbilityInstance::NotifyAbilityEnded()
{
	if (OwningAbility) OwningAbility->NotifyAbilityInstanceEnded(this);
}
