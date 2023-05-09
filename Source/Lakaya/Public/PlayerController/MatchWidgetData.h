// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MatchWidgetData.generated.h"

/**
 * 
 */
USTRUCT()
struct LAKAYA_API FMatchWidgetData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UMatchStateWidget>> WidgetList;
};
