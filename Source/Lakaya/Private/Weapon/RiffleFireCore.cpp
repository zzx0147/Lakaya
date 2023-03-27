// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RiffleFireCore.h"

#include "Weapon/GunComponent.h"
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

void URiffleFireCore::FireStartCore(FTimerHandle& FireTimer, const EFocusContext& FocusContext,
                                    const uint16& FireCount, std::function<void()> OnContinuousFire,
                                    std::function<void()> OnFreshFire, std::function<void()> OnElse)
{
	auto IsFocusable = Character->SetFocus(FocusContext, EFocusSpace::MainHand, EFocusState::Firing);
	if (GetWorld()->GetTimerManager().IsTimerActive(FireTimer)) { if (FireCount == 0) OnContinuousFire(); }
	else if (IsFocusable) OnFreshFire();
	else if (OnElse) OnElse();
	else UE_LOG(LogNetSubObject, Log, TEXT("FireStartCore skipped!"));
}

void URiffleFireCore::FireStopCore(const EGunSelector& Selector, uint16& FireCount, const EFocusContext& FocusContext,
                                   std::function<void()> OnStop)
{
	if (Selector == EGunSelector::Auto)
	{
		if (Character->ReleaseFocus(FocusContext, EFocusSpace::MainHand, EFocusState::Firing))
		{
			FireCount = 0;
			if (OnStop) OnStop();
		}
		else UE_LOG(LogNetSubObject, Error, TEXT("Fail to release focus on FireStopCore with %d context!"),
		            FocusContext);
	}
}

void URiffleFireCore::SwitchSelectorCore(EGunSelector& DesiredSelector, EGunSelector& Selector,
                                         FTimerHandle& SelectorTimer, const EFocusContext& FocusContext,
                                         std::function<void()> OnDesiredSelectorUpdated)
{
	auto& TimerManager = GetWorld()->GetTimerManager();

	// 조정간을 조정중이지 않으면서 Focusable도 아니라면 스킵합니다.
	if (!Character->SetFocus(FocusContext, EFocusSpace::MainHand, EFocusState::Switching)
		&& !TimerManager.IsTimerActive(SelectorTimer))
		return;

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

	TimerManager.SetTimer(SelectorTimer, [this,&DesiredSelector,&Selector,FocusContext]
	{
		if (Character->ReleaseFocus(FocusContext, EFocusSpace::MainHand, EFocusState::Switching))
		{
			Selector = DesiredSelector;
			GEngine->AddOnScreenDebugMessage(-1, 3, GetDebugColor(FocusContext),TEXT("Selector updated"));
		}
		else UE_LOG(LogNetSubObject, Error,
		            TEXT("Fail to release focus on UpdateSelector with %d context!"), FocusContext);
	}, SwitchingDelay, false);
	if (OnDesiredSelectorUpdated) OnDesiredSelectorUpdated();
	GEngine->AddOnScreenDebugMessage(-1, 3, GetDebugColor(FocusContext),TEXT("Switch timer setted!"));
}

void URiffleFireCore::FreshFireCore(const EGunSelector& Selector, uint16& FireCount, FTimerHandle& FireTimer,
                                    std::function<void()> RepeatFireFunction)
{
	SetFireCount(Selector, FireCount);
	GetWorld()->GetTimerManager().SetTimer(FireTimer, RepeatFireFunction, FireDelay, true, 0.f);
}

void URiffleFireCore::FireCallback(uint16& FireCount, FTimerHandle& FireTimer, const EFocusContext& FocusContext,
                                   std::function<bool()> EmptyPredicate, std::function<void()> OnEmpty,
                                   std::function<void()> OnSingleFire)
{
	if (FireCount == 1)
	{
		if (!Character->ReleaseFocus(FocusContext, EFocusSpace::MainHand, EFocusState::Firing))
			UE_LOG(LogNetSubObject, Error, TEXT("Fail to release focus on FireCallback with %d context!"), FocusContext);
	}
	else if (FireCount == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimer);
		GEngine->AddOnScreenDebugMessage(-1, 3, GetDebugColor(FocusContext),TEXT("FireTimer expired!"));
		return;
	}
	--FireCount;

	if (EmptyPredicate())
	{
		FireCount = 0;
		if (OnEmpty) OnEmpty();
		return;
	}

	if (OnSingleFire) OnSingleFire();
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
		UE_LOG(LogNetSubObject, Error, TEXT("Selector was not EFireMode!"));
		return;
	}
}

FColor URiffleFireCore::GetDebugColor(const EFocusContext& FocusContext)
{
	switch (FocusContext)
	{
	case EFocusContext::Server: return FColor::White;
	case EFocusContext::Simulated: return FColor::Green;
	case EFocusContext::Owner: return FColor::Yellow;
	default: return FColor::Black;
	}
}
