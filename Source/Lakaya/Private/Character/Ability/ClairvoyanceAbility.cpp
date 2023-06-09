// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/ClairvoyanceAbility.h"

#include "ETC/OutlineManager.h"
#include "Kismet/GameplayStatics.h"

UClairvoyanceAbility::UClairvoyanceAbility() : Super()
{
	bCanEverStartRemoteCall = bUpdateStartTimeOnStart = true;
	AbilityStartDelay = 0.2f;
	BaseAbilityDuration = 30.f;
	BaseCoolTime = 5.f;
}

void UClairvoyanceAbility::OnAliveStateChanged(const bool& AliveState)
{
	Super::OnAliveStateChanged(AliveState);
	if (auto& TimerManager = GetWorld()->GetTimerManager(); !AliveState && TimerManager.TimerExists(ClairvoyanceTimer))
	{
		TimerManager.ClearTimer(ClairvoyanceTimer);
		DisableClairvoyance();
	}
}

TObjectPtr<AOutlineManager> UClairvoyanceAbility::GetOutlineManager()
{
	if (OutlineManager.IsValid()) return OutlineManager.Get();
	OutlineManager = Cast<AOutlineManager>(UGameplayStatics::GetActorOfClass(this, AOutlineManager::StaticClass()));
	return OutlineManager.Get();
}

void UClairvoyanceAbility::DisableClairvoyance()
{
	if (GetOwner()->HasAuthority()) ApplyCoolTime();
	GetOutlineManager()->UnRegisterClairvoyance(GetUniqueID(), GetPlayerTeam());
}

bool UClairvoyanceAbility::ShouldStartRemoteCall()
{
	return !GetWorld()->GetTimerManager().TimerExists(ClairvoyanceTimer) && IsEnableTime(GetServerTime() + 0.1f);
}

void UClairvoyanceAbility::StartDelayedAbility()
{
	Super::StartDelayedAbility();
	GetOutlineManager()->RegisterClairvoyance(GetUniqueID(), GetPlayerTeam());
	GetWorld()->GetTimerManager().SetTimer(ClairvoyanceTimer, this, &UClairvoyanceAbility::DisableClairvoyance,
	                                       BaseAbilityDuration);
}
