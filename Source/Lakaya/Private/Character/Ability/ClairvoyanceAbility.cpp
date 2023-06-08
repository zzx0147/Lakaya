// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/ClairvoyanceAbility.h"

#include "ETC/OutlineManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UClairvoyanceAbility::UClairvoyanceAbility() : Super()
{
	bUseDelayedAbility = true;
	DelayedAbilityDuration = 30.0f;
	InitialDelay = 0.2f;
	bIsClairvoyanceOn = false;
	SetClairvoyanceState(bIsClairvoyanceOn);
}

void UClairvoyanceAbility::SetClairvoyanceState(const bool& NewState)
{
	if (NewState == bIsClairvoyanceOn) return;
	bIsClairvoyanceOn = NewState;
	OnClairvoyanceChanged.Broadcast(bIsClairvoyanceOn);
}

void UClairvoyanceAbility::OnAliveStateChanged(const bool& AliveState)
{
	Super::OnAliveStateChanged(AliveState);
	if (!AliveState)
	{
		StopDelayedAbility();
	}
}

TObjectPtr<AOutlineManager> UClairvoyanceAbility::GetOutlineManager()
{
	if (OutlineManager.IsValid()) return OutlineManager.Get();
	OutlineManager = Cast<AOutlineManager>(UGameplayStatics::GetActorOfClass(this, AOutlineManager::StaticClass()));
	return OutlineManager.Get();
}

bool UClairvoyanceAbility::ShouldStartRemoteCall()
{
	if (IsDelayedAbilityRunning() || GetWorld()->GetTimerManager().TimerExists(AbilityStartTimerHandle)) return false;
	
	return IsEnableTime(GetServerTime() + 0.1f);
}

void UClairvoyanceAbility::StartDelayedAbility()
{
	Super::StartDelayedAbility();
	GetOutlineManager()->RegisterClairvoyance(GetUniqueID(), GetPlayerTeam());
}

void UClairvoyanceAbility::StopDelayedAbility()
{
	Super::StopDelayedAbility();
	if (GetOwner()->HasAuthority()) ApplyCoolTime();
	GetOutlineManager()->UnRegisterClairvoyance(GetUniqueID(), GetPlayerTeam());;
	SetClairvoyanceState(false);
}

void UClairvoyanceAbility::OnDelayedAbilityStartTimeChanged(const float& NewDelayedAbilityStartTime)
{
	Super::OnDelayedAbilityStartTimeChanged(NewDelayedAbilityStartTime);
	if(NewDelayedAbilityStartTime < 0) return;
	SetClairvoyanceState(true);
}

