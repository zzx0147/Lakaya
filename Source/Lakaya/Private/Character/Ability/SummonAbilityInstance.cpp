// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/SummonAbilityInstance.h"

ASummonAbilityInstance::ASummonAbilityInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
}

void ASummonAbilityInstance::OnSummoned()
{
}

void ASummonAbilityInstance::BroadcastOnAbilityEnded()
{
	OnAbilityEnded.Broadcast(this);
}
