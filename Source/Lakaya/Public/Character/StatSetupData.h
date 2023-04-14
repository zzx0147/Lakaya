#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StatSetupData.generated.h"


USTRUCT()
struct FStatSetupData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere)
	float BaseRunAdditive;

	UPROPERTY(EditAnywhere)
	float BaseMaximumHealth;
};