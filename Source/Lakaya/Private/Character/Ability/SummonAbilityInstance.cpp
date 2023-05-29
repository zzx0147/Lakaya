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
	if (PerformDelay <= 0.f) return;
	if (HasAuthority()) AbilityTime = GetServerTime() + PerformDelay;
	GetWorld()->GetTimerManager().SetTimer(PerformTimer, this, &ASummonAbilityInstance::PerformTimerHandler,
	                                       AbilityTime - GetServerTime());
}

void ASummonAbilityInstance::HandleAbilityInstanceEnding()
{
	if (CollapseDelay <= 0.f || !HasAuthority()) return;
	GetWorld()->GetTimerManager().SetTimer(CollapseTimer, this, &ASummonAbilityInstance::CollapseTimerHandler,
	                                       CollapseDelay);
}

float ASummonAbilityInstance::GetServerTime() const
{
	return GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
}

void ASummonAbilityInstance::PerformTimerHandler()
{
	if (HasAuthority()) SetAbilityInstanceState(EAbilityInstanceState::Perform);
}

void ASummonAbilityInstance::CollapseTimerHandler()
{
	SetAbilityInstanceState(EAbilityInstanceState::Collapsed);
}
