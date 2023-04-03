// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WeaponFireData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponFireData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Fire)
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Fire)
	float FireRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Fire)
	float BaseDamage;
};