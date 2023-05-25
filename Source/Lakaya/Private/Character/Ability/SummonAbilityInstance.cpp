// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/SummonAbilityInstance.h"

#include "Character/Ability/CoolTimedSummonAbility.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

ASummonAbilityInstance::ASummonAbilityInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
	PerformDelay = 0.2f;
	CollapseDelay = 0.2f;
}

void ASummonAbilityInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASummonAbilityInstance, OwningAbility)
	DOREPLIFETIME(ASummonAbilityInstance, AbilityInstanceState)
	DOREPLIFETIME(ASummonAbilityInstance, AbilityTime)
}

void ASummonAbilityInstance::SetAbilityInstanceState(const EAbilityInstanceState& DesireState)
{
	if (AbilityInstanceState == DesireState) return;
	AbilityInstanceState = DesireState;
	OnRep_AbilityInstanceState();
}

void ASummonAbilityInstance::OnRep_AbilityInstanceState()
{
	switch (AbilityInstanceState)
	{
	case EAbilityInstanceState::Collapsed:
		HandleAbilityInstanceCollapsed();
		break;
	case EAbilityInstanceState::Ready:
		HandleAbilityInstanceReady();
		break;
	case EAbilityInstanceState::Perform:
		HandleAbilityInstancePerform();
		break;
	case EAbilityInstanceState::Ending:
		HandleAbilityInstanceEnding();
		break;
	default:
		UE_LOG(LogScript, Error, TEXT("AbilityInstanceState was invalid state! it was %d"), AbilityInstanceState);
		break;
	}
}

void ASummonAbilityInstance::HandleAbilityInstanceReady()
{
	if (PerformDelay <= 0.f) return;
	if (HasAuthority()) AbilityTime = GetServerTime() + PerformDelay;
	GetWorld()->GetTimerManager().SetTimer(PerformTimer, this, &ASummonAbilityInstance::PerformTimerHandler,
	                                       AbilityTime - GetServerTime());
	if (OwningAbility) OwningAbility->NotifyPerformTime(AbilityTime);
}

void ASummonAbilityInstance::HandleAbilityInstanceEnding()
{
	if (PerformDelay <= 0.f || !HasAuthority()) return;
	GetWorld()->GetTimerManager().SetTimer(CollapseTimer, this, &ASummonAbilityInstance::CollapseTimerHandler,
	                                       CollapseDelay);
}

void ASummonAbilityInstance::HandleAbilityInstanceCollapsed()
{
	if (HasAuthority() && OwningAbility) OwningAbility->NotifyAbilityInstanceCollapsed(this);
}

float ASummonAbilityInstance::GetServerTime() const
{
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void ASummonAbilityInstance::PerformTimerHandler()
{
	SetAbilityInstanceState(EAbilityInstanceState::Perform);
}

void ASummonAbilityInstance::CollapseTimerHandler()
{
	SetAbilityInstanceState(EAbilityInstanceState::Collapsed);
}
