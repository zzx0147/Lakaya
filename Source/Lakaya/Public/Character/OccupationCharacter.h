// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/InteractableCharacter.h"
#include "Character/OccupationPlayerState.h"
#include "OccupationCharacter.generated.h"

UCLASS()
class LAKAYA_API AOccupationCharacter : public AInteractableCharacter
{
	GENERATED_BODY()

public:
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

	// 대상 액터가 같은 팀인지 판별합니다.
	bool IsSameTeam(AActor* const& Other) const;

	void SetTeam(const EPlayerTeamState& Team);
};
