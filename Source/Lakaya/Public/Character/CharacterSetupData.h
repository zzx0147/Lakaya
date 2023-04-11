#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterSetupData.generated.h"


USTRUCT()
struct FCharacterSetupData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> MeshActorClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCharacterAbility> PrimaryClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCharacterAbility> SecondClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCharacterAbility> FireClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCharacterAbility> AbilityClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCharacterAbility> ReloadClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UResourceComponent> ResourceClass;
};
