// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RenaCharacter.h"

#include "Character/BulletComponent.h"
#include "Character/Ability/CoolTimedSummonAbility.h"
#include "Character/Ability/DeathRayAbility.h"
#include "Character/Ability/ReloadAbility.h"
#include "Character/Ability/ResultNotifyFireAbility.h"


ARenaCharacter::ARenaCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer
	.SetDefaultSubobjectClass(AbilityComponentNames[Primary], UCoolTimedSummonAbility::StaticClass()).
	SetDefaultSubobjectClass(AbilityComponentNames[Secondary], UDeathRayAbility::StaticClass()).
	SetDefaultSubobjectClass(AbilityComponentNames[WeaponFire], UResultNotifyFireAbility::StaticClass()).
	SetDefaultSubobjectClass(AbilityComponentNames[WeaponAbility], UCoolTimedSummonAbility::StaticClass()).
	SetDefaultSubobjectClass(AbilityComponentNames[WeaponReload], UReloadAbility::StaticClass()).
	SetDefaultSubobjectClass(ResourceComponentName, UBulletComponent::StaticClass()))
{
	MineLateDelay = 0.3f;
	BombLateDelay = 0.3f;

	const auto MineAbility = FindAbility<UCoolTimedSummonAbility>(Primary);
	const auto FireAbility = FindAbility<UResultNotifyFireAbility>(WeaponFire);
	const auto BombAbility = FindAbility<UCoolTimedSummonAbility>(WeaponAbility);
	const auto ReloadAbility = FindAbility<UReloadAbility>(WeaponReload);

	MineAbility->OnPerformTimeNotified.AddUObject(this, &ARenaCharacter::OnMinePerformTimeNotified);
	FireAbility->OnWantsToFireChanged.AddUObject(this, &ARenaCharacter::OnWantsToFireChanged);
	BombAbility->OnPerformTimeNotified.AddUObject(this, &ARenaCharacter::OnBombPerformTimeNotified);
	ReloadAbility->OnReloadStateChanged.AddUObject(this, &ARenaCharacter::OnReloadStateChanged);
	ReloadAbility->OnReloadCompleteTimeNotified.AddUObject(this, &ARenaCharacter::OnReloadCompleteTimeNotified);
}

bool ARenaCharacter::ShouldStartAbility_Implementation(EAbilityKind Kind)
{
	// 네트워크 레이턴시를 고려하여 기준 시간을 현재보다 조금 더 늦게 잡아서 조금 더 이른 시점부터 능력 사용 요청을 보낼 수 있도록 합니다. 
	const auto BasisTime = GetServerTime() + 0.1f;

	// 생존중이고, 점착지뢰 투척이 끝날 때까지 얼마 안남았고, 폭탄 투척이 끝날 때까지 얼마 안남았고, 재장전 종료까지 얼마 안남았고, 사격중이지 않을 때 스킬을 사용할 수 있도록 합니다. 
	return GetAliveState() && MineEndingTime <= BasisTime && BombEndingTime <= BasisTime
		&& ReloadCompleteTime <= BasisTime && !bWantsToFire;
}

bool ARenaCharacter::ShouldStartAbilityOnServer_Implementation(EAbilityKind Kind)
{
	const auto CurrentTime = GetServerTime();

	// 생존중이고, 점착지뢰 투척이 끝났고, 폭탄 투척이 끝났고, 재장전중이지 않고, 사격중이지 않을 때 스킬을 사용할 수 있도록 합니다.
	return GetAliveState() && MineEndingTime <= CurrentTime && BombEndingTime <= CurrentTime && !bIsReloading
		&& !bWantsToFire;
}

void ARenaCharacter::OnMinePerformTimeNotified(const float& Time)
{
	MineEndingTime = Time + MineLateDelay;
}

void ARenaCharacter::OnWantsToFireChanged(bool FireState)
{
	bWantsToFire = FireState;
}

void ARenaCharacter::OnBombPerformTimeNotified(const float& Time)
{
	BombEndingTime = Time + BombLateDelay;
}

void ARenaCharacter::OnReloadCompleteTimeNotified(const float& Time)
{
	ReloadCompleteTime = Time;
}

void ARenaCharacter::OnReloadStateChanged(bool ReloadState)
{
	bIsReloading = ReloadState;
}
