// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WeaponAssetData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponAssetData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Fire)
	TSoftClassPtr<class UWeaponFire> FireClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Fire)
	FName FireRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Ability)
	TSoftClassPtr<class UWeaponAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Ability)
	FName AbilityRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Reload)
	TSoftClassPtr<class UWeaponReload> ReloadClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Reload)
	FName ReloadRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UDataTable> UpgradeTable;
};
