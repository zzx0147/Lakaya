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
	virtual ETeam GetTeam() const = 0;

	FORCEINLINE bool IsSameTeam(const ETeam& InTeam) const
	{
		return JudgeSameTeam(GetTeam(), InTeam);
	}

	FORCEINLINE bool IsSameTeam(const ITeamObjectInterface* InTeamObject) const
	{
		return InTeamObject && JudgeSameTeam(GetTeam(), InTeamObject->GetTeam());
	}
};

FORCEINLINE bool IsSameTeam(const UObject* InTeamObject, const UObject* InOtherTeamObject)
{
	const auto TeamObject = Cast<ITeamObjectInterface>(InTeamObject);
	return TeamObject && TeamObject->IsSameTeam(Cast<ITeamObjectInterface>(InOtherTeamObject));
}
