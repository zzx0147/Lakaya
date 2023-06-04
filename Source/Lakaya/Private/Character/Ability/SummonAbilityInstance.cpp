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

void ASummonAbilityInstance::SetOwningAbility(UCoolTimedSummonAbility* const& Ability)
{
	OwningAbility = Ability;
	OnRep_OwningAbility();
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
	// 같은 스테이트로의 전환 시도이거나, 부적절한 스테이트로의 전환 시도인 경우 넘깁니다.
	if (AbilityInstanceState == DesireState || DesireState < EAbilityInstanceState::Collapsed
		|| DesireState > EAbilityInstanceState::Ending)
		return;
	AbilityInstanceState = DesireState;
	OnRep_AbilityInstanceState();
}

void ASummonAbilityInstance::SetStateTimer(void (ASummonAbilityInstance::*TimerHandler)(), const float& Delay)
{
	const auto Time = GetServerTime();
	if (HasAuthority()) AbilityTime = Time + Delay;
	GetWorld()->GetTimerManager().SetTimer(StateTimer, this, TimerHandler, AbilityTime - Time);
}

void ASummonAbilityInstance::SetStateTimer(const EAbilityInstanceState& HandlerKey, const float& Delay)
{
	switch (HandlerKey)
	{
	case EAbilityInstanceState::Collapsed:
		SetStateTimer(&ASummonAbilityInstance::CollapseTimerHandler, Delay);
		break;
	case EAbilityInstanceState::Ready:
		SetStateTimer(&ASummonAbilityInstance::ReadyTimerHandler, Delay);
		break;
	case EAbilityInstanceState::Perform:
		SetStateTimer(&ASummonAbilityInstance::PerformTimerHandler, Delay);
		break;
	case EAbilityInstanceState::ReadyForAction:
		SetStateTimer(&ASummonAbilityInstance::ReadyForActionTimerHandler, Delay);
		break;
	case EAbilityInstanceState::Action:
		SetStateTimer(&ASummonAbilityInstance::ActionTimerHandler, Delay);
		break;
	case EAbilityInstanceState::Ending:
		SetStateTimer(&ASummonAbilityInstance::EndingTimerHandler, Delay);
		break;
	default: ;
	}
}

void ASummonAbilityInstance::SetTeam(const EPlayerTeam& Team)
{
	RecentTeam = Team;
}

void ASummonAbilityInstance::OnRep_AbilityInstanceState()
{
	const auto PrevInstanceState = RecentInstanceState;
	RecentInstanceState = AbilityInstanceState;
	switch (PrevInstanceState)
	{
	case EAbilityInstanceState::Collapsed:
		HandleCollapsedStateExit();
		break;
	case EAbilityInstanceState::Ready:
		HandleReadyStateExit();
		break;
	case EAbilityInstanceState::Perform:
		HandlePerformStateExit();
		break;
	case EAbilityInstanceState::ReadyForAction:
		HandleReadyForActionStateExit();
		break;
	case EAbilityInstanceState::Action:
		HandleActionStateExit();
		break;
	case EAbilityInstanceState::Ending:
		HandleEndingStateExit();
		break;
	}
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
	case EAbilityInstanceState::ReadyForAction:
		HandleAbilityInstanceReadyForAction();
		break;
	case EAbilityInstanceState::Action:
		HandleAbilityInstanceAction();
		break;
	case EAbilityInstanceState::Ending:
		HandleAbilityInstanceEnding();
		break;
	}
	OnAbilityInstanceStateChanged.Broadcast(AbilityInstanceState, this);
}

void ASummonAbilityInstance::OnRep_OwningAbility()
{
	OnAbilityInstanceStateChanged.AddUObject(OwningAbility.Get(),
	                                         &UCoolTimedSummonAbility::NotifyAbilityInstanceStateChanged);
}

void ASummonAbilityInstance::HandleAbilityInstanceReady()
{
	if (PerformDelay > 0.f) SetStateTimer(&ASummonAbilityInstance::PerformTimerHandler, PerformDelay);
}

void ASummonAbilityInstance::HandleAbilityInstanceReadyForAction()
{
	if (ActionDelay > 0.f) SetStateTimer(&ASummonAbilityInstance::ActionTimerHandler, ActionDelay);
}

void ASummonAbilityInstance::HandleAbilityInstanceEnding()
{
	if (CollapseDelay > 0.f) SetStateTimer(&ASummonAbilityInstance::CollapseTimerHandler, CollapseDelay);
}

float ASummonAbilityInstance::GetServerTime() const
{
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void ASummonAbilityInstance::ReadyTimerHandler()
{
	if (HasAuthority()) SetAbilityInstanceState(EAbilityInstanceState::Ready);
}

void ASummonAbilityInstance::PerformTimerHandler()
{
	if (HasAuthority()) SetAbilityInstanceState(EAbilityInstanceState::Perform);
}

void ASummonAbilityInstance::ReadyForActionTimerHandler()
{
	if (HasAuthority()) SetAbilityInstanceState(EAbilityInstanceState::ReadyForAction);
}

void ASummonAbilityInstance::ActionTimerHandler()
{
	if (HasAuthority()) SetAbilityInstanceState(EAbilityInstanceState::Action);
}

void ASummonAbilityInstance::EndingTimerHandler()
{
	if (HasAuthority()) SetAbilityInstanceState(EAbilityInstanceState::Ending);
}

void ASummonAbilityInstance::CollapseTimerHandler()
{
	if (HasAuthority()) SetAbilityInstanceState(EAbilityInstanceState::Collapsed);
}
