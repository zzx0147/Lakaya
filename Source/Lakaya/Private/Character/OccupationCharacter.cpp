// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OccupationCharacter.h"


float AOccupationCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	// 같은 팀인 경우 무시합니다.
	if (IsSameTeam(DamageCauser)) return 0.f;
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

bool AOccupationCharacter::IsSameTeam(AActor* const& Other) const
{
	if (const auto CastedState = GetPlayerState<AOccupationPlayerState>())
		return CastedState->IsSameTeam(Other);
	return false;
}

void AOccupationCharacter::SetTeam(const EPlayerTeamState& Team)
{
	USkeletalMeshComponent* LocalMesh = GetMesh();
	if (Team == EPlayerTeamState::A) LocalMesh->SetMaterial(0, LocalMesh->GetMaterial(1));
	else if (Team == EPlayerTeamState::B) LocalMesh->SetMaterial(0, LocalMesh->GetMaterial(2));
}