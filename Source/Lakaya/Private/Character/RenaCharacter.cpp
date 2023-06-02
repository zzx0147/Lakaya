// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RenaCharacter.h"

#include "Character/BulletComponent.h"
#include "Character/Ability/CoolTimedSummonAbility.h"
#include "Character/Ability/DeathRayAbility.h"
#include "Character/Ability/ReloadAbility.h"
#include "Character/Ability/ResultNotifyFireAbility.h"


ARenaCharacter::ARenaCharacter(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass(AbilityComponentNames[Primary], UCoolTimedSummonAbility::StaticClass()).
	                  SetDefaultSubobjectClass(AbilityComponentNames[Secondary], UDeathRayAbility::StaticClass()).
	                  SetDefaultSubobjectClass(AbilityComponentNames[WeaponFire],
	                                           UResultNotifyFireAbility::StaticClass()).
	                  SetDefaultSubobjectClass(AbilityComponentNames[WeaponAbility],
	                                           UCoolTimedSummonAbility::StaticClass()).
	                  SetDefaultSubobjectClass(AbilityComponentNames[WeaponReload], UReloadAbility::StaticClass()).
	                  SetDefaultSubobjectClass(ResourceComponentName, UBulletComponent::StaticClass()))
{
	MineLateDelay = 0.3f;
	BombLateDelay = 0.3f;

	const auto PrimaryAbility = FindAbility<UCoolTimedSummonAbility>(Primary);
	const auto FireAbility = FindAbility<UResultNotifyFireAbility>(WeaponFire);
	const auto WeaponSkill = FindAbility<UCoolTimedSummonAbility>(WeaponAbility);
	const auto ReloadAbility = FindAbility<UReloadAbility>(WeaponReload);

	PrimaryAbility->OnPerformTimeNotified.AddUObject(this, &ARenaCharacter::OnPrimaryPerformTimeNotified);
	FireAbility->OnWantsToFireChanged.AddUObject(this, &ARenaCharacter::OnWantsToFireChanged);
	WeaponSkill->OnPerformTimeNotified.AddUObject(this, &ARenaCharacter::OnWeaponAbilityPerformTimeNotified);
	ReloadAbility->OnReloadStateChanged.AddUObject(this, &ARenaCharacter::OnReloadStateChanged);
	ReloadAbility->OnReloadCompleteTimeNotified.AddUObject(this, &ARenaCharacter::OnReloadCompleteTimeNotified);
}

bool ARenaCharacter::ShouldStartAbility_Implementation(EAbilityKind Kind)
{
	// 네트워크 레이턴시를 고려하여 기준 시간을 현재보다 조금 더 늦게 잡아서 조금 더 이른 시점부터 능력 사용 요청을 보낼 수 있도록 합니다. 
	const auto BasisTime = GetServerTime() + 0.1f;
	return GetAliveState() && PrimaryEndingTime <= BasisTime && WeaponAbilityEndingTime <= BasisTime
		&& ReloadCompleteTime <= BasisTime && !WantsToFire;
}

bool ARenaCharacter::ShouldStartAbilityOnServer_Implementation(EAbilityKind Kind)
{
	const auto CurrentTime = GetServerTime();
	return GetAliveState() && PrimaryEndingTime <= CurrentTime && WeaponAbilityEndingTime <= CurrentTime
		&& !bIsReloading && !WantsToFire;
}

void ARenaCharacter::OnPrimaryPerformTimeNotified(const float& Time)
{
	PrimaryEndingTime = Time + MineLateDelay;
}

void ARenaCharacter::OnWantsToFireChanged(bool FireState)
{
	WantsToFire = FireState;
}

void ARenaCharacter::OnWeaponAbilityPerformTimeNotified(const float& Time)
{
	WeaponAbilityEndingTime = Time + BombLateDelay;
}

void ARenaCharacter::OnReloadCompleteTimeNotified(const float& Time)
{
	ReloadCompleteTime = Time;
}

void ARenaCharacter::OnReloadStateChanged(bool ReloadState)
{
	bIsReloading = ReloadState;
}
