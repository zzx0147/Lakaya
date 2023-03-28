#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Weapon/UpgradeTypeEnum.h"
#include "WeaponUpgradeData.generated.h"

USTRUCT(BlueprintType)
struct FWeaponUpgradeData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Fire)
	int32 UpgradeNum;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Upgrade)
	UpgradeTypeEnum UpgrageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Upgrade)
	FString UpgrageData;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Fire)
	FName FireRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	TSoftClassPtr<class UWeaponAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	FName AbilityRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Reload)
	TSoftClassPtr<class UWeaponReload> ReloadClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Reload)
	FName ReloadRowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UStaticMesh> WeaponMesh;*/
};
