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

void URiffleFireCore::FireStartCore(FTimerHandle& GunSelectorTimer, FTimerHandle& GunFireTimer,
                                    std::function<bool()> EmptyDeterminant, std::function<void()> OnEmptyMag,
                                    std::function<void()> NestedFire, std::function<void()> FreshFire)
{
	auto& TimerManager = GetWorld()->GetTimerManager();

	if (TimerManager.IsTimerActive(GunSelectorTimer))
		return;

	if (EmptyDeterminant())
	{
		if (OnEmptyMag != nullptr) OnEmptyMag();
		return;
	}

	if (TimerManager.IsTimerActive(GunFireTimer)) NestedFire();
	else FreshFire();
}

void URiffleFireCore::FireStopCore(const EGunSelector& GunSelector, uint16& GunFireCount)
{
	if (GunSelector == EGunSelector::Auto) GunFireCount = 0;
}

void URiffleFireCore::SwitchSelectorCore(const uint16& GunFireCount, EGunSelector& GunSelector,
										   FTimerHandle& GunSelectorTimer, std::function<void()> OnUpdateSelector)
{
	if (GunFireCount != 0) return;

	if (GunComponent.IsValid()) GunComponent->SetReloadEnabled(false);

	switch (GunSelector)
	{
	case EGunSelector::Semi: GunSelector = EGunSelector::Burst;
		break;
	case EGunSelector::Burst: GunSelector = EGunSelector::Auto;
		break;
	case EGunSelector::Auto: GunSelector = EGunSelector::Semi;
		break;
	default:
		UE_LOG(LogActorComponent, Error, TEXT("DesiredFire was not EFireMode"));
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(GunSelectorTimer, OnUpdateSelector, SwitchingDelay - LockstepDelay, false);
}

void URiffleFireCore::NestedFireCore(const EGunSelector& GunSelector, uint16& GunFireCount)
{
	switch (GunSelector)
	{
	case EGunSelector::Semi:
		if (GunFireCount == 0) GunFireCount = 1;
		break;
	case EGunSelector::Burst:
		if (GunFireCount == 0) GunFireCount = 3;
		break;
	case EGunSelector::Auto:
		GunFireCount = MAX_uint16;
		break;
	default:
		UE_LOG(LogActorComponent, Error, TEXT("GunSelector was not EFireMode"));
		return;
	}
}

void URiffleFireCore::FreshFireCore(const EGunSelector& GunSelector, uint16& GunFireCount,
									  FTimerHandle& FireTimerHandle,
									  std::function<void()> FireFunction)
{
	switch (GunSelector)
	{
	case EGunSelector::Semi: GunFireCount = 1;
		break;
	case EGunSelector::Burst: GunFireCount = 3;
		break;
	case EGunSelector::Auto: GunFireCount = MAX_uint16;
		break;
	default:
		UE_LOG(LogActorComponent, Error, TEXT("GunSelector was not EFireMode"));
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, FireFunction, FireDelay, true, 0.f);
}

void URiffleFireCore::FireCallback(uint16& GunFireCount, FTimerHandle& FireTimerHandle,
									 std::function<bool()> EmptyDeterminant,
									 std::function<void()> OnEmpty, std::function<void()> FireFunction)
{
	if (GunFireCount == 0)
	{
		StopFireCore(GunFireCount, FireTimerHandle);
		return;
	}
	--GunFireCount;

	if (EmptyDeterminant())
	{
		if (OnEmpty != nullptr) OnEmpty();
		StopFireCore(GunFireCount, FireTimerHandle);
		return;
	}

	FireFunction();
}

void URiffleFireCore::StopFireCore(uint16& GunFireCount, FTimerHandle& FireTimerHandle)
{
	GunFireCount = 0;
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}