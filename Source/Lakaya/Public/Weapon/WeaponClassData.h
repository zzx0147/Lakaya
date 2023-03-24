// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/Object.h"
#include "WeaponClassData.generated.h"

/**
 * 
 */
USTRUCT()
struct FWeaponClassData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSoftClassPtr<class UWeaponComponent> WeaponClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FName AssetRowName;
};
