// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/OverdriveAbility.h"

#include "Character/BuffPlayerState.h"

UOverdriveAbility::UOverdriveAbility()
{
	bCanEverStartRemoteCall = bUpdateStartTimeOnStart = true;
	AbilityStartDelay = 0.2f;
	BaseCoolTime = 5.f;
	BaseAbilityDuration = 8.f;
}

void UOverdriveAbility::SetEffectMaterial(UMaterialInstanceDynamic* NewEffectMaterial)
{
	EffectMaterial = NewEffectMaterial;
}

void UOverdriveAbility::StartDelayedAbility()
{
	Super::StartDelayedAbility();
	ApplyBuff();
	GetWorld()->GetTimerManager().SetTimer(OverdriveTimer, this, &UOverdriveAbility::DisableOverdrive,
	                                       BaseAbilityDuration);
	if (EffectMaterial.IsValid()) EffectMaterial->SetScalarParameterValue("OverdriveEffectOpacity", 0.2f);
}

void UOverdriveAbility::OnAliveStateChanged(const bool& AliveState)
{
	Super::OnAliveStateChanged(AliveState);
	if (auto& TimerManager = GetWorld()->GetTimerManager(); !AliveState && TimerManager.TimerExists(OverdriveTimer))
	{
		TimerManager.ClearTimer(OverdriveTimer);
		DisableOverdrive();
	}
}

bool UOverdriveAbility::ShouldStartRemoteCall()
{
	return !GetWorld()->GetTimerManager().TimerExists(OverdriveTimer) && IsEnableTime(GetServerTime() + 0.1f);
}

void UOverdriveAbility::ApplyBuff()
{
	if (!GetOwner()->HasAuthority()) return;

	if (const auto MyCharacter = GetOwner<APawn>())
		if (const auto BuffPlayerState = MyCharacter->GetPlayerState<ABuffPlayerState>())
			BuffPlayerState->ApplyBuff(TEXT("Overdrive"));
}

void UOverdriveAbility::DisableOverdrive()
{
	if (GetOwner()->HasAuthority()) ApplyCoolTime();
	if (EffectMaterial.IsValid()) EffectMaterial->SetScalarParameterValue("OverdriveEffectOpacity", 0.0f);
}
