#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Weapon/UpgradeTypeEnum.h"
#include "WeaponUpgradeData.generated.h"


USTRUCT(BlueprintType)
struct FWeaponUpgradeElement
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Upgrade)
	UpgradeTypeEnum UpgradeType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Upgrade)
	FString UpgradeData;
};


USTRUCT(BlueprintType)
struct FWeaponUpgradeData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Upgrade)
	TArray<FWeaponUpgradeElement> UpgradeDataArray;
};
