// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OccupationCharacter.h"

#include "Character/OccupationPlayerState.h"
#include "GameMode/LakayaDefaultPlayGameMode.h"


float AOccupationCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                       AController* EventInstigator, AActor* DamageCauser)
{
	// 같은 팀인 경우 무시합니다.
	if (IsSameTeam(DamageCauser)) return 0.f;
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AOccupationCharacter::KillCharacter(AController* EventInstigator, AActor* DamageCauser)
{
	Super::KillCharacter(EventInstigator, DamageCauser);
	if (const auto GameMode = GetWorld()->GetAuthGameMode<ALakayaDefaultPlayGameMode>())
		GameMode->OnKilledCharacter(GetController(), this, EventInstigator, DamageCauser);
}

bool AOccupationCharacter::IsSameTeam(AActor* const& Other) const
{
	if (const auto CastedState = GetPlayerState<AOccupationPlayerState>())
		return CastedState->IsSameTeam(Other);
	return false;
}

void AOccupationCharacter::SetTeam(const EPlayerTeam& Team)
{
	USkeletalMeshComponent* LocalMesh = GetMesh();
	if (Team == EPlayerTeam::A) LocalMesh->SetMaterial(0, LocalMesh->GetMaterial(1));
	else if (Team == EPlayerTeam::B) LocalMesh->SetMaterial(0, LocalMesh->GetMaterial(2));
}
