#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterWidgetComponentClassData.generated.h"

USTRUCT()
struct FCharacterWidgetComponentClassData : public FTableRowBase
{
	GENERATED_BODY()

public:
	TSubclassOf<class UCharacterWidgetComponent> WidgetComponentClass;
};
