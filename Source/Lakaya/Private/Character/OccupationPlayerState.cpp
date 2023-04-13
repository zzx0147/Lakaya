// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/OccupationPlayerState.h"

#include "Character/OccupationCharacter.h"
#include "Net/UnrealNetwork.h"


void AOccupationPlayerState::BeginPlay()
{
	Super::BeginPlay();
	OnPawnSet.AddDynamic(this, &AOccupationPlayerState::OnPawnSetCallback);
}

void AOccupationPlayerState::SetPlayerTeamState(const EPlayerTeamState& TeamState)
{
	PlayerTeamState = TeamState;
	if (const auto CastedPawn = GetPawn<AOccupationCharacter>())
		CastedPawn->SetTeam(PlayerTeamState);
}

bool AOccupationPlayerState::IsSameTeam(AActor* const& Other) const
{
	if (const auto CastedPawn = Cast<APawn>(Other))
		if (const auto OtherState = CastedPawn->GetPlayerState<AOccupationPlayerState>())
			return IsSameTeam(OtherState);
	return false;
}

void AOccupationPlayerState::OnPawnSetCallback(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (const auto CastedPawn = Cast<AOccupationCharacter>(NewPawn))
		CastedPawn->SetTeam(PlayerTeamState);
}

bool AOccupationPlayerState::IsSameTeam(APlayerState* const& Other) const
{
	if (const auto CastedState = Cast<AOccupationPlayerState>(Other))
		return IsSameTeam(CastedState);
	return false;
}

bool AOccupationPlayerState::IsSameTeam(AOccupationPlayerState* const& Other) const
{
	if (Other && Other->PlayerTeamState != EPlayerTeamState::None)
		return PlayerTeamState == Other->PlayerTeamState;
	return false;
}

void AOccupationPlayerState::OnRep_PlayerTeamState()
{
	if (const auto CastedPawn = Cast<AOccupationCharacter>(GetPawn()))
		CastedPawn->SetTeam(PlayerTeamState);
}

void AOccupationPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOccupationPlayerState, PlayerTeamState);
}
