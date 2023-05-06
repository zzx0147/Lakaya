// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ArmedCharacter.h"

#include "Character/Ability/CharacterAbility.h"
#include "Net/UnrealNetwork.h"

void AArmedCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArmedCharacter, Abilities);
}

ELifetimeCondition AArmedCharacter::AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const
{
	if (ComponentToReplicate->IsA(UCharacterAbility::StaticClass())) return COND_None;
	return Super::AllowActorComponentToReplicate(ComponentToReplicate);
}

void AArmedCharacter::StartAbility(const EAbilityKind& Kind)
{
	if (Abilities.IsValidIndex(Kind))
		if (const auto& Ability = Abilities[Kind])
			Ability->AbilityStart();
}

void AArmedCharacter::StopAbility(const EAbilityKind& Kind)
{
	if (Abilities.IsValidIndex(Kind))
		if (const auto& Ability = Abilities[Kind])
			Ability->AbilityStop();
}
