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
		Abilities.EmplaceAt(Index, Cast<UCharacterAbility>(CreateDefaultSubobject(
			                    AbilityComponentNames[Index], UCharacterAbility::StaticClass(),
			                    UCharacterAbility::StaticClass(), false, false)));
	}
}

ELifetimeCondition AArmedCharacter::AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const
{
	if (ComponentToReplicate->IsA(UCharacterAbility::StaticClass())) return COND_None;
	return Super::AllowActorComponentToReplicate(ComponentToReplicate);
}

void AArmedCharacter::SetTeam_Implementation(const EPlayerTeam& Team)
{
	Super::SetTeam_Implementation(Team);
	for (const auto& Ability : Abilities) Ability->SetTeam(Team);
}

void AArmedCharacter::SetAliveState_Implementation(bool IsAlive)
{
	Super::SetAliveState_Implementation(IsAlive);
	for (const auto& Ability : Abilities) Ability->OnAliveStateChanged(IsAlive);
}

void AArmedCharacter::StartAbility(const EAbilityKind& Kind)
{
	if (const auto Ability = FindAbility(Kind); Ability && ShouldStartAbility(Kind))
	{
		Ability->LocalAbilityStart();
		if (Ability->CanStartRemoteCall())
			RequestStartAbility(Kind, GetServerTime());
	}
}

void AArmedCharacter::StopAbility(const EAbilityKind& Kind)
{
	if (const auto Ability = FindAbility(Kind); Ability && ShouldStopAbility(Kind))
	{
		Ability->LocalAbilityStop();
		if (Ability->CanStopRemoteCall())
			RequestStopAbility(Kind, GetServerTime());
	}
}

UCharacterAbility* AArmedCharacter::FindAbility(const EAbilityKind& Kind) const
{
	if (Abilities.IsValidIndex(Kind)) return Abilities[Kind];
	return nullptr;
}

bool AArmedCharacter::ShouldStopAbilityOnServer_Implementation(EAbilityKind Kind)
{
	return true;
}

bool AArmedCharacter::ShouldStartAbilityOnServer_Implementation(EAbilityKind Kind)
{
	return true;
}

void AArmedCharacter::RequestStopAbility_Implementation(const EAbilityKind& Kind, const float& Time)
{
	if (ShouldStopAbilityOnServer(Kind)) Abilities[Kind]->RemoteAbilityStop(Time);
}

bool AArmedCharacter::RequestStopAbility_Validate(const EAbilityKind& Kind, const float& Time)
{
	// Time값이 조작되었는지 여부를 검사합니다. 0.05f는 서버시간의 허용오차를 의미합니다.
	// return FindAbility(Kind) != nullptr;
	// return FindAbility(Kind) && GetServerTime() + 0.07f >= Time;
	return true;
}

void AArmedCharacter::RequestStartAbility_Implementation(const EAbilityKind& Kind, const float& Time)
{
	if (ShouldStartAbilityOnServer(Kind)) Abilities[Kind]->RemoteAbilityStart(Time);
}

bool AArmedCharacter::RequestStartAbility_Validate(const EAbilityKind& Kind, const float& Time)
{
	// Time값이 조작되었는지 여부를 검사합니다. 0.05f는 서버시간의 허용오차를 의미합니다.
	// return FindAbility(Kind) != nullptr;
	// return FindAbility(Kind) && GetServerTime() + 0.7f >= Time;
	return true;
}

bool AArmedCharacter::ShouldStartAbility_Implementation(EAbilityKind Kind)
{
	return true;
}

bool AArmedCharacter::ShouldStopAbility_Implementation(EAbilityKind Kind)
{
	return true;
}
