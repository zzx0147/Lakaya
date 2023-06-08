// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/OverdriveAbility.h"

#include "Character/BuffPlayerState.h"
#include "Character/StatPlayerState.h"
#include "GameFramework/Character.h"

UOverdriveAbility::UOverdriveAbility()
{
	bUseDelayedAbility = true;
	bCanEverStartRemoteCall = true;
	bIsOverdriveOn = false;
}

void UOverdriveAbility::SetEffectMaterial(UMaterialInstanceDynamic* NewEffectMaterial)
{
	EffectMaterial = NewEffectMaterial;
}

void UOverdriveAbility::OnDelayedAbilityStartTimeChanged(const float& NewDelayedAbilityStartTime)
{
	Super::OnDelayedAbilityStartTimeChanged(NewDelayedAbilityStartTime);

	SetOverdriveState(true);
}

void UOverdriveAbility::StartDelayedAbility()
{
	Super::StartDelayedAbility();
	if (EffectMaterial.IsValid()) EffectMaterial->SetScalarParameterValue("EffectOpacity", 0.2f);

	SetOverdriveState(false);

	ApplyBuff();
}

void UOverdriveAbility::StopDelayedAbility()
{
	Super::StopDelayedAbility();
	if (EffectMaterial.IsValid()) EffectMaterial->SetScalarParameterValue("EffectOpacity", 0.0f);

	ApplyCoolTime();
}

void UOverdriveAbility::SetOverdriveState(const bool& NewState)
{
	if (NewState == bIsOverdriveOn) return;
	bIsOverdriveOn = NewState;
	OnOverdriveChanged.Broadcast(bIsOverdriveOn);
}

void UOverdriveAbility::OnAliveStateChanged(const bool& AliveState)
{
	Super::OnAliveStateChanged(AliveState);
	StopDelayedAbility();
}

bool UOverdriveAbility::ShouldStartRemoteCall()
{
	if (IsDelayedAbilityRunning() || GetWorld()->GetTimerManager().TimerExists(AbilityStartTimerHandle)) return false;
	return IsEnableTime(GetServerTime() + 0.1f);
}

void UOverdriveAbility::ApplyBuff()
{
	if (!GetOwner()->HasAuthority()) return;

	if (const auto MyCharacter = Cast<ACharacter>(GetOwner()))
		if (const auto BuffPlayerState = MyCharacter->GetPlayerState<ABuffPlayerState>())
			BuffPlayerState->ApplyBuff(TEXT("Overdrive"));
	
}

void UOverdriveAbility::ExpireBuff()
{
	if (!GetOwner()->HasAuthority()) return;
	
	if (const auto MyCharacter = Cast<ACharacter>(GetOwner()))
		if (const auto BuffPlayerState = MyCharacter->GetPlayerState<ABuffPlayerState>())
			BuffPlayerState->ExpireBuff(TEXT("Overdrive"));
}
