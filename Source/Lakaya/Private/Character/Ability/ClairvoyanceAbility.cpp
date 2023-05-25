// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Ability/ClairvoyanceAbility.h"

#include "Character/LakayaBaseCharacter.h"
#include "Character/LakayaBasePlayerState.h"
#include "ETC/OutlineManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UClairvoyanceAbility::UClairvoyanceAbility() : Super()
{
	AbilityDuration = 1.0f;
}

void UClairvoyanceAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UClairvoyanceAbility, AbilityStartTime);
}

void UClairvoyanceAbility::OnRep_AbilityStartTime()
{
	if(AbilityStartTime > GetServerTime())
	{
		GetWorld()->GetTimerManager().SetTimer(AbilityStartHandle,[this]
		{
		
			const auto OutlineManagerRef = GetOutlineManager();
			if(const auto Character = Cast<ALakayaBaseCharacter>(GetOwner()))
			{
				if(const auto BasePlayerState = Character->GetPlayerState<ALakayaBasePlayerState>())
				{
					OutlineManagerRef->RegisterClairvoyance(GetUniqueID(),BasePlayerState->GetTeam());

				
					GetWorld()->GetTimerManager().SetTimer(AbilityEndHandle,[this,OutlineManagerRef,BasePlayerState]
					{
						OutlineManagerRef->UnRegisterClairvoyance(GetUniqueID(),BasePlayerState->GetTeam());
					},AbilityStartTime + AbilityDuration - GetServerTime() ,false);
				}
			}
		},AbilityStartTime - GetServerTime(),false);	
	}
	else
	{
		const auto OutlineManagerRef = GetOutlineManager();
		if(const auto Character = Cast<ALakayaBaseCharacter>(GetOwner()))
		{
			if(const auto BasePlayerState = Character->GetPlayerState<ALakayaBasePlayerState>())
			{
				OutlineManagerRef->RegisterClairvoyance(GetUniqueID(),BasePlayerState->GetTeam());

				
				GetWorld()->GetTimerManager().SetTimer(AbilityEndHandle,[this,OutlineManagerRef,BasePlayerState]
				{
					OutlineManagerRef->UnRegisterClairvoyance(GetUniqueID(),BasePlayerState->GetTeam());
				},AbilityStartTime + AbilityDuration - GetServerTime() ,false);
			}
		}
	}
}

void UClairvoyanceAbility::RemoteAbilityStart(const float& RequestTime)
{
	Super::RemoteAbilityStart(RequestTime);
	AbilityStartTime = RequestTime + AbilityDelay;
	OnRep_AbilityStartTime();
}

void UClairvoyanceAbility::OnAliveStateChanged(const bool& AliveState)
{
	Super::OnAliveStateChanged(AliveState);
}

void UClairvoyanceAbility::InitializeComponent()
{
	Super::InitializeComponent();
}

TObjectPtr<AOutlineManager> UClairvoyanceAbility::GetOutlineManager()
{
	if(OutlineManager.IsValid()) return OutlineManager.Get();
	
	OutlineManager = Cast<AOutlineManager>(UGameplayStatics::GetActorOfClass(this,AOutlineManager::StaticClass()));


	return OutlineManager.Get();
}
