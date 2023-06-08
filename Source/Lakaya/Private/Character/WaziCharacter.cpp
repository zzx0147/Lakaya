// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/WaziCharacter.h"

#include "Character/Ability/OverdriveAbility.h"
#include "Character/Ability/ClairvoyanceAbility.h"
#include "Character/Ability/CoolTimedSummonAbility.h"
#include "Character/Ability/ReloadAbility.h"
#include "Character/Ability/ResultNotifyFireAbility.h"

AWaziCharacter::AWaziCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.
	SetDefaultSubobjectClass(AbilityComponentNames[Primary], UOverdriveAbility::StaticClass()).
	SetDefaultSubobjectClass(AbilityComponentNames[Secondary], UClairvoyanceAbility::StaticClass()).
	SetDefaultSubobjectClass(AbilityComponentNames[WeaponFire], UResultNotifyFireAbility::StaticClass()).
	SetDefaultSubobjectClass(AbilityComponentNames[WeaponAbility], UCoolTimedSummonAbility::StaticClass()).
	SetDefaultSubobjectClass(AbilityComponentNames[WeaponReload], UReloadAbility::StaticClass()))
{
	const auto OverdriveAbility = FindAbility<UOverdriveAbility>(Primary);
	const auto ClairvoyanceAbility = FindAbility<UClairvoyanceAbility>(Secondary);
	const auto FireAbility = FindAbility<UResultNotifyFireAbility>(WeaponFire);
	const auto SmokeAbility = FindAbility<UCoolTimedSummonAbility>(WeaponAbility);
	const auto ReloadAbility = FindAbility<UReloadAbility>(WeaponReload);

	//TODO: 투시 능력의 선딜레이, 후딜레이 시간동안 행동할 수 없도록 해야 합니다.
	FireAbility->OnWantsToFireChanged.AddUObject(this, &AWaziCharacter::OnWantsToFireChanged);
	SmokeAbility->OnPerformTimeNotified.AddUObject(this, &AWaziCharacter::OnSmokePerformTimeNotified);
	ReloadAbility->OnReloadStateChanged.AddUObject(this, &AWaziCharacter::OnReloadStateChanged);
	ReloadAbility->OnReloadCompleteTimeNotified.AddUObject(this, &AWaziCharacter::OnReloadCompleteTimeNotified);
}

bool AWaziCharacter::ShouldStartAbility_Implementation(EAbilityKind Kind)
{
	// 네트워크 레이턴시를 고려하여 기준 시간을 현재보다 조금 더 늦게 잡아서 조금 더 이른 시점부터 능력 사용 요청을 보낼 수 있도록 합니다. 
	const auto BasisTime = GetServerTime() + 0.1f;

	// 생존하고 있고, 연막 투척 스킬 종료까지 얼마 안남았고, 재장전 종료까지 얼마 안남았고, 사격중이지 않을 때 스킬을 사용할 수 있게 합니다.
	return GetAliveState() && SmokeEndingTime <= BasisTime && ReloadCompleteTime <= BasisTime && !bWantsToFire;
}

bool AWaziCharacter::ShouldStartAbilityOnServer_Implementation(EAbilityKind Kind)
{
	// 생존하고 있고, 연막 투척 스킬이 종료된 시점이고, 재장전중이지 않고 사격중이지 않을 때에 스킬을 사용할 수 있게 합니다.
	return GetAliveState() && SmokeEndingTime <= GetServerTime() && !bIsReloading && !bWantsToFire;
}

void AWaziCharacter::OnWantsToFireChanged(bool FireState)
{
	bWantsToFire = FireState;
}

void AWaziCharacter::OnSmokePerformTimeNotified(const float& Time)
{
	SmokeEndingTime = Time + SmokeLateDelay;
}

void AWaziCharacter::OnReloadStateChanged(bool ReloadState)
{
	bIsReloading = ReloadState;
}

void AWaziCharacter::OnReloadCompleteTimeNotified(const float& Time)
{
	ReloadCompleteTime = Time;
}
