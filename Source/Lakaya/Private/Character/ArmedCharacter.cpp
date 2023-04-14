// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ArmedCharacter.h"

#include "Character/CharacterSetupData.h"
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

void AArmedCharacter::KillCharacter(AController* EventInstigator, AActor* DamageCauser)
{
	Super::KillCharacter(EventInstigator, DamageCauser);
	for (const auto& Ability : Abilities)
		Ability->OnCharacterDead();
}

void AArmedCharacter::SetupCharacterServer(const FCharacterSetupData* Data)
{
	Super::SetupCharacterServer(Data);

	Abilities.Reserve(5);
	Abilities.EmplaceAt(Primary, CreateReplicatedComponent(Data->PrimaryClass));
	Abilities.EmplaceAt(Secondary, CreateReplicatedComponent(Data->SecondClass));
	Abilities.EmplaceAt(WeaponFire, CreateReplicatedComponent(Data->FireClass));
	Abilities.EmplaceAt(WeaponAbility, CreateReplicatedComponent(Data->AbilityClass));
	Abilities.EmplaceAt(WeaponReload, CreateReplicatedComponent(Data->ReloadClass));
	OnAbilitiesChanged.Broadcast(Abilities);
}

void AArmedCharacter::StartAbility(const EAbilityKind& Kind)
{
	//TODO: 입력이 순서대로 실행되게 하려면, Reliable RPC가 어빌리티 내에서 호출되는 것이 아니라 여기에서 호출되어야 합니다.
	if (Abilities.Contains(Kind)) Abilities[Kind]->AbilityStart();
}

void AArmedCharacter::StopAbility(const EAbilityKind& Kind)
{
	if (Abilities.Contains(Kind)) Abilities[Kind]->AbilityStop();
}

void AArmedCharacter::OnRep_Abilities()
{
	OnAbilitiesChanged.Broadcast(Abilities);
}
