// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/RiffleFireOwner.h"

#include "Weapon/GunComponent.h"

void URiffleFireOwner::FireStart()
{
	FireStartCore(FireTimer, EFocusContext::Owner, FireCount, [this]
	{
		Super::FireStart();
		SetFireCount(Selector, FireCount);
	}, [this]
	{
		Super::FireStart();
		FreshFireCore(Selector, FireCount, FireTimer, [this]
		{
			FireCallback(FireCount, FireTimer, EFocusContext::Owner, [this]
			{
				return GunComponent->GetRemainBullets() <= 0;
			}, [this]
			{
				if (Character->ReleaseFocus(EFocusContext::Owner, EFocusSpace::MainHand, EFocusState::Firing))
					GunComponent->ReloadStart();
				else UE_LOG(LogNetSubObject, Error, TEXT("Fail to release focus on FireStart!"));
			});
		});
	});
}

void URiffleFireOwner::FireStop()
{
	FireStopCore(Selector, FireCount, EFocusContext::Owner, [this] { Super::FireStop(); });
}

void URiffleFireOwner::SwitchSelector()
{
	SwitchSelectorCore(DesiredSelector, Selector, SelectorTimer, EFocusContext::Owner,
	                   [this] { Super::SwitchSelector(); });
}
