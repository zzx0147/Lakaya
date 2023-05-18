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
	if (!ShouldStartAbility(Kind)) return;

	const auto& Ability = Abilities[Kind];
	Ability->LocalAbilityStart();

	if (Ability->CanStartRemoteCall())
		RequestStartAbility(Kind, GetServerTime());
}

void AArmedCharacter::StopAbility(const EAbilityKind& Kind)
{
	if (!ShouldStopAbility(Kind)) return;

	const auto& Ability = Abilities[Kind];
	Ability->LocalAbilityStop();

	if (Ability->CanStopRemoteCall())
		RequestStopAbility(Kind, GetServerTime());
}

void AArmedCharacter::RequestStopAbility_Implementation(const EAbilityKind& Kind, const float& Time)
{
	if (ShouldStopAbilityOnServer(Kind)) Abilities[Kind]->RemoteAbilityStop(Time);
}

bool AArmedCharacter::RequestStopAbility_Validate(const EAbilityKind& Kind, const float& Time)
{
	// Time값이 조작되었는지 여부를 검사합니다. 0.05f는 서버시간의 허용오차를 의미합니다.
	return Abilities.IsValidIndex(Kind) && Abilities[Kind] && GetServerTime() + 0.05f >= Time;
}

void AArmedCharacter::RequestStartAbility_Implementation(const EAbilityKind& Kind, const float& Time)
{
	if (ShouldStartAbilityOnServer(Kind)) Abilities[Kind]->RemoteAbilityStart(Time);
}

bool AArmedCharacter::RequestStartAbility_Validate(const EAbilityKind& Kind, const float& Time)
{
	// Time값이 조작되었는지 여부를 검사합니다. 0.05f는 서버시간의 허용오차를 의미합니다.
	return Abilities.IsValidIndex(Kind) && Abilities[Kind] && GetServerTime() + 0.05f >= Time;
}

bool AArmedCharacter::ShouldStartAbility(const EAbilityKind& Kind)
{
	return Abilities.IsValidIndex(Kind) && Abilities[Kind];
}

bool AArmedCharacter::ShouldStopAbility(const EAbilityKind& Kind)
{
	return Abilities.IsValidIndex(Kind) && Abilities[Kind];
}

bool AArmedCharacter::ShouldStartAbilityOnServer(const EAbilityKind& Kind)
{
	return true;
}

bool AArmedCharacter::ShouldStopAbilityOnServer(const EAbilityKind& Kind)
{
	return true;
}
