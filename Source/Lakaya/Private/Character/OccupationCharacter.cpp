// Fill out your copyright notice in the Description page of Project Settings.


#include "OccupationCharacter.h"

#include "Character/CollectorPlayerState.h"

void AOccupationCharacter::OnPlayerStateChanged(APlayerState* NewState)
{
	if (const auto CollectorState = Cast<ACollectorPlayerState>(NewState))
	{
		OnTeamChanged(CollectorState->GetPlayerTeamState());
		CollectorState->OnTeamChanged.AddUObject(this, &AOccupationCharacter::OnTeamChanged);
	}
}

void AOccupationCharacter::OnTeamChanged(const EPlayerTeamState& Team)
{
	USkeletalMeshComponent* LocalMesh = GetMesh();
	if (Team == EPlayerTeamState::A) LocalMesh->SetMaterial(0, LocalMesh->GetMaterial(1));
	else if (Team == EPlayerTeamState::B) LocalMesh->SetMaterial(0, LocalMesh->GetMaterial(2));
}
