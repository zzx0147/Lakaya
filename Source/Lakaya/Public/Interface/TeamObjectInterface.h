// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Occupation/Team.h"
#include "UObject/Interface.h"
#include "TeamObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTeamObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LAKAYA_API ITeamObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual ETeam GetTeam() = 0;
};
