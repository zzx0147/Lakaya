// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAssetData.h"
#include "Engine/DataTable.h"
#include "GunAssetData.generated.h"

/**
 * 
 */
USTRUCT()
struct FGunAssetData : public FWeaponAssetData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	uint16 Magazine;
};
