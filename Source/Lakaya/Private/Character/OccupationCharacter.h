// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CollectorPlayerState.h"
#include "Character/InteractableCharacter.h"
#include "OccupationCharacter.generated.h"

UCLASS()
class LAKAYA_API AOccupationCharacter : public AInteractableCharacter
{
	GENERATED_BODY()

public:
	AOccupationCharacter(const FObjectInitializer& ObjectInitializer);
	
public:
	void SetTeam(const EPlayerTeamState& Team);
};
