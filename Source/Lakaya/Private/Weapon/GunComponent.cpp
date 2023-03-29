// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GunComponent.h"

#include "Weapon/GunAssetData.h"
#include "Weapon/WeaponUpgradeData.h"
#include "Weapon/RiffleFireCore.h"
#include "Engine/DataTable.h"
#include "Net/UnrealNetwork.h"



void UGunComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGunComponent, MagazineCapacity);
	DOREPLIFETIME(UGunComponent, RemainBullets);
}


UGunComponent::UGunComponent()
{
	static const ConstructorHelpers::FObjectFinder<UDataTable> DataFinder(
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/GunAssetDataTable'"));

	if (DataFinder.Succeeded()) WeaponAssetDataTable = DataFinder.Object;
}

void UGunComponent::Reload()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Reloading"));
	RemainBullets = MagazineCapacity;
}

void UGunComponent::Reload(const uint16& Bullets)
{
	RemainBullets += Bullets;
	if (RemainBullets > MagazineCapacity) RemainBullets = MagazineCapacity;
}

bool UGunComponent::CostBullets(const uint16& Bullets)
{
	if (Bullets > RemainBullets) return false;
	RemainBullets -= Bullets;
	return true;
}

void UGunComponent::UpgradeWeapon()
{
	Super::UpgradeWeapon();
	FWeaponUpgradeData* Data = WeaponUpgradeDataTable->FindRow<FWeaponUpgradeData>(FName(FString::FromInt(UpgradeLevel)), TEXT("GunComponentUpgradeWeapon"));

	for (auto temp : Data->UpgradeDataArray)
	{
		switch (temp.UpgradeType)
		{
		case UpgradeTypeEnum::Bullet:
		{
			MagazineCapacity += (int8)FCString::Atoi(*temp.UpgradeData);
			break;
		}
		case UpgradeTypeEnum::Damage:
		{
			auto RiffleFireCoreRef = Cast<URiffleFireCore>(FireSubObject);
			float AdditionalDamage = AdditionalDamage = FCString::Atof(*temp.UpgradeData);
			if (RiffleFireCoreRef != nullptr)
				RiffleFireCoreRef->SetBaseDamage(RiffleFireCoreRef->GetBaseDamage() + AdditionalDamage);
			break;
		}
		case UpgradeTypeEnum::Range:
		{
			UE_LOG(LogTemp, Warning, TEXT("RangeUpgrade was not Developed"));
			break;
		}
		case UpgradeTypeEnum::SkillDamage:
		{
			UE_LOG(LogTemp, Warning, TEXT("RangeUpgrade was not Developed"));
			break;
		}
		case UpgradeTypeEnum::SkillRange:
		{
			UE_LOG(LogTemp, Warning, TEXT("RangeUpgrade was not Developed"));
			break;
		}
		}
	}

}

void UGunComponent::SetupData()
{
	Super::SetupData();
	auto Data = WeaponAssetDataTable->FindRow<FGunAssetData>(RequestedRowName, TEXT("GunComponent"));
	if (!Data) return;

	RemainBullets = MagazineCapacity = OriginMagazineCapacity = Data->Magazine;
}
