// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ArmedCharacter.h"

#include "Character/Ability/CharacterAbility.h"
#include "Net/UnrealNetwork.h"

const TArray<FName> AArmedCharacter::AbilityComponentNames = {
	FName(TEXT("PrimaryAbility")), FName(TEXT("SecondaryAbility")), FName(TEXT("WeaponFire")),
	FName(TEXT("WeaponAbility")), FName(TEXT("WeaponReload")), FName(TEXT("DashAbility"))
};

void AArmedCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArmedCharacter, Abilities);
}

AArmedCharacter::AArmedCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Abilities.Reserve(EAbilityKind::Count);
	for (auto Index = 0; Index < EAbilityKind::Count; ++Index)
	{
		const auto Ability = CreateDefaultSubobject<UCharacterAbility>(AbilityComponentNames[Index]);
		Ability->SetIsReplicated(true);
		Abilities.EmplaceAt(Index, Ability);
	}
}

ELifetimeCondition AArmedCharacter::AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const
{
	if (ComponentToReplicate->IsA(UCharacterAbility::StaticClass())) return COND_None;
	return Super::AllowActorComponentToReplicate(ComponentToReplicate);
}

void AArmedCharacter::SetAliveState_Implementation(bool IsAlive)
{
	Super::SetAliveState_Implementation(IsAlive);
	for (const auto& Ability : Abilities) Ability->OnAliveStateChanged(IsAlive);
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
