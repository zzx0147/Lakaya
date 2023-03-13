// Fill out your copyright notice in the Description page of Project Settings.


#include "RiffleFireCore.h"

#include "GunComponent.h"
#include "Net/UnrealNetwork.h"

void URiffleFireCore::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(URiffleFireCore, BaseDamage, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFireCore, FireDelay, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFireCore, FireRange, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFireCore, SqrFireRange, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFireCore, SwitchingDelay, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFireCore, GunComponent, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(URiffleFireCore, Character, COND_InitialOnly);
}

void URiffleFireCore::FireStartCore(FTimerHandle& SelectorTimer, FTimerHandle& FireTimer,
                                    std::function<void()> OnEmptyMag,
                                    std::function<void()> OnNestedFire, std::function<void()> OnFreshFire)
{
	auto& TimerManager = GetWorld()->GetTimerManager();

	if (TimerManager.IsTimerActive(SelectorTimer))
		return;

	if (GunComponent.IsValid() && GunComponent->GetRemainBullets() <= 0)
	{
		if (OnEmptyMag != nullptr) OnEmptyMag();
		return;
	}

	if (TimerManager.IsTimerActive(FireTimer)) OnNestedFire();
	else OnFreshFire();
}

void URiffleFireCore::FireStopCore(const EGunSelector& Selector, uint16& FireCount)
{
	if (Selector == EGunSelector::Auto) FireCount = 0;
}

void URiffleFireCore::SwitchSelectorCore(const uint16& FireCount, EGunSelector& DesiredSelector,
                                         FTimerHandle& SelectorTimer, std::function<void()> OnUpdateSelector)
{
	if (FireCount != 0) return;

	if (GunComponent.IsValid()) GunComponent->SetReloadEnabled(false);

	switch (DesiredSelector)
	{
	case EGunSelector::Semi: DesiredSelector = EGunSelector::Burst;
		break;
	case EGunSelector::Burst: DesiredSelector = EGunSelector::Auto;
		break;
	case EGunSelector::Auto: DesiredSelector = EGunSelector::Semi;
		break;
	default:
		UE_LOG(LogActorComponent, Error, TEXT("DesiredFire was not EFireMode"));
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(SelectorTimer, OnUpdateSelector, SwitchingDelay - LockstepDelay, false);
}

void URiffleFireCore::NestedFireCore(const EGunSelector& Selector, uint16& FireCount)
{
	switch (Selector)
	{
	case EGunSelector::Semi:
		if (FireCount == 0) FireCount = 1;
	case EGunSelector::Burst:
		if (FireCount == 0) FireCount = 3;
		if (GunComponent.IsValid()) GunComponent->SetReloadEnabled(false);
		break;
	case EGunSelector::Auto:
		FireCount = MAX_uint16;
		break;
	default:
		UE_LOG(LogActorComponent, Error, TEXT("GunSelector was not EFireMode"));
		return;
	}
}

void URiffleFireCore::FreshFireCore(const EGunSelector& Selector, uint16& FireCount, FTimerHandle& FireTimer,
                                    std::function<void()> RepeatFireFunction)
{
	switch (Selector)
	{
	case EGunSelector::Semi: FireCount = 1;
		break;
	case EGunSelector::Burst: FireCount = 3;
		break;
	case EGunSelector::Auto: FireCount = MAX_uint16;
		break;
	default:
		UE_LOG(LogActorComponent, Error, TEXT("GunSelector was not EFireMode"));
		return;
	}

	if (GunComponent.IsValid()) GunComponent->SetReloadEnabled(false);
	GetWorld()->GetTimerManager().SetTimer(FireTimer, RepeatFireFunction, FireDelay, true, 0.f);
}

void URiffleFireCore::FireCallback(uint16& FireCount, FTimerHandle& FireTimer, std::function<bool()> EmptyDeterminant,
                                   std::function<void()> OnEmpty, std::function<void()> OnSingleFire)
{
	if (FireCount == 0)
	{
		StopFireCore(FireCount, FireTimer);
		return;
	}
	--FireCount;
	if (FireCount == 0 && GunComponent.IsValid()) GunComponent->SetReloadEnabled(true);

	if (EmptyDeterminant())
	{
		if (GunComponent.IsValid()) GunComponent->SetReloadEnabled(true);
		if (OnEmpty != nullptr) OnEmpty();
		StopFireCore(FireCount, FireTimer);
		return;
	}

	OnSingleFire();
}

void URiffleFireCore::StopFireCore(uint16& FireCount, FTimerHandle& FireTimer)
{
	FireCount = 0;
	GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}

void URiffleFireCore::UpdateSelector(EGunSelector& DesiredSelector, EGunSelector& Selector)
{
	Selector = DesiredSelector;
	if (GunComponent.IsValid()) GunComponent->SetReloadEnabled(true);
}
