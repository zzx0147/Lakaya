// Fill out your copyright notice in the Description page of Project Settings.


#include "RiffleFireCore.h"

#include "GunComponent.h"
#include "FocusableCharacter.h"
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

void URiffleFireCore::FireStartCore(FTimerHandle& FireTimer, std::function<bool()>
                                    FocusableDeterminant, std::function<void()> OnContinuousFire,
                                    std::function<void()> OnFreshFire)
{
	if (FocusableDeterminant()) OnFreshFire();
	else if (GetWorld()->GetTimerManager().IsTimerActive(FireTimer)) OnContinuousFire();
}

void URiffleFireCore::FireStopCore(const EGunSelector& Selector, uint16& FireCount, const bool& IsSimulated)
{
	if (Selector == EGunSelector::Auto)
	{
		FireCount = 0;
		if (!Character->ReleaseFocus(IsSimulated ? EFocusContext::Simulated : EFocusContext::Server,
		                             EFocusSpace::MainHand, EFocusState::Firing))
			UE_LOG(LogNetSubObject, Error, TEXT("Fail to release focus on FireStopCore!"))
	}
}

void URiffleFireCore::SwitchSelectorCore(EGunSelector& DesiredSelector, FTimerHandle& SelectorTimer,
                                         std::function<void()> OnUpdateSelector,
                                         std::function<bool()> NotFocusableDeterminant)
{
	auto& TimerManager = GetWorld()->GetTimerManager();
	if (NotFocusableDeterminant() && !TimerManager.IsTimerActive(SelectorTimer))
	{
		UE_LOG(LogNetSubObject, Log, TEXT("Skip switching selector"));
		return;
	}

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

	TimerManager.SetTimer(SelectorTimer, OnUpdateSelector, SwitchingDelay - LockstepDelay, false);
	GEngine->AddOnScreenDebugMessage(-1, 3, Character->HasAuthority() ? FColor::White : FColor::Red,
	                                 TEXT("Switch timer setted"));
}

void URiffleFireCore::ContinuousFireCore(const EGunSelector& Selector, uint16& FireCount)
{
	if (FireCount != 0) return;
	SetFireCount(Selector, FireCount);
	UE_LOG(LogNetSubObject, Log, TEXT("FireCount setted by NestedFireCore"));
}

void URiffleFireCore::FreshFireCore(const EGunSelector& Selector, uint16& FireCount, FTimerHandle& FireTimer,
                                    std::function<void()> RepeatFireFunction)
{
	SetFireCount(Selector, FireCount);
	GetWorld()->GetTimerManager().SetTimer(FireTimer, RepeatFireFunction, FireDelay, true, 0.f);
	GEngine->AddOnScreenDebugMessage(-1, 3, Character->HasAuthority() ? FColor::White : FColor::Red,
	                                 TEXT("FireTimer setted"));
}

void URiffleFireCore::FireCallback(uint16& FireCount, FTimerHandle& FireTimer, std::function<bool()> EmptyDeterminant,
                                   std::function<void()> OnEmpty, std::function<void()> OnSingleFire,
                                   std::function<void()> OnFirePreEnding)
{
	if (FireCount == 1) OnFirePreEnding();
	else if (FireCount == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimer);
		GEngine->AddOnScreenDebugMessage(-1, 3, Character->HasAuthority() ? FColor::White : FColor::Red,
		                                 TEXT("FireTimer expired"));
		return;
	}
	--FireCount;

	if (EmptyDeterminant())
	{
		FireCount = 0;
		if (OnEmpty != nullptr) OnEmpty();
		return;
	}

	OnSingleFire();
}

void URiffleFireCore::UpdateSelector(EGunSelector& DesiredSelector, EGunSelector& Selector, const bool& IsSimulated)
{
	Selector = DesiredSelector;
	if (!Character->ReleaseFocus(IsSimulated ? EFocusContext::Simulated : EFocusContext::Server,
	                             EFocusSpace::MainHand, EFocusState::Switching))
		UE_LOG(LogNetSubObject, Error, TEXT("Fail to release focus on UpdateSelector!"));
	GEngine->AddOnScreenDebugMessage(-1, 3, IsSimulated ? FColor::Red : FColor::White,
	                                 TEXT("Selector updated"));
}

void URiffleFireCore::SetFireCount(const EGunSelector& Selector, uint16& FireCount)
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
}
