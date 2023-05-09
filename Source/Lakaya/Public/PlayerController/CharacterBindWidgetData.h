#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterBindWidgetData.generated.h"

USTRUCT()
struct FCharacterBindWidgetData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UCharacterBindableWidget>> WidgetList;
};
