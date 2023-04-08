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
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/DT_GunAssetDataTable'"));

	if (DataFinder.Succeeded()) WeaponAssetDataTable = DataFinder.Object;
}

void UGunComponent::Reload()
{
	RemainBullets = MagazineCapacity;
	OnCurrentBulletChanged.Broadcast(RemainBullets);
}

void UGunComponent::Reload(const uint16& Bullets)
{
	RemainBullets += Bullets;
	if (RemainBullets > MagazineCapacity) RemainBullets = MagazineCapacity;
	OnCurrentBulletChanged.Broadcast(RemainBullets);
}

bool UGunComponent::CostBullets(const uint16& Bullets)
{
	if (Bullets > RemainBullets) return false;
	RemainBullets -= Bullets;
	OnCurrentBulletChanged.Broadcast(RemainBullets);
	return true;
}

void UGunComponent::UpgradeWeapon()
{
	Super::UpgradeWeapon();

	if (UpgradeLevel > 4) return;

	FWeaponUpgradeData* Data = WeaponUpgradeDataTable->FindRow<FWeaponUpgradeData>(
		FName(FString::FromInt(UpgradeLevel)), TEXT("GunComponentUpgradeWeapon"));

	for (auto temp : Data->UpgradeDataArray)
	{
		switch (temp.UpgradeType)
		{
		case UpgradeTypeEnum::Bullet:
			{
				MagazineCapacity += (int8)FCString::Atoi(*temp.UpgradeData);
				OnMaximumBulletChanged.Broadcast(MagazineCapacity);
				break;
			}
		case UpgradeTypeEnum::Damage:
			{
				const auto RiffleFireCoreRef = Cast<URiffleFireCore>(FireSubObject);
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

void UGunComponent::UpgradeInitialize()
{
	Super::UpgradeInitialize();
	RemainBullets = MagazineCapacity = OriginMagazineCapacity;
	OnCurrentBulletChanged.Broadcast(RemainBullets);
	OnMaximumBulletChanged.Broadcast(MagazineCapacity);

	const auto RiffleFireCoreRef = Cast<URiffleFireCore>(FireSubObject);
	if (RiffleFireCoreRef != nullptr)
		RiffleFireCoreRef->SetBaseDamage(RiffleFireCoreRef->GetOriginBaseDamage());
}

void UGunComponent::SetupData()
{
	Super::SetupData();
	const auto Data = WeaponAssetDataTable->FindRow<FGunAssetData>(RequestedRowName, TEXT("GunComponent"));
	if (!Data) return;

	RemainBullets = MagazineCapacity = OriginMagazineCapacity = Data->Magazine;
	OnCurrentBulletChanged.Broadcast(RemainBullets);
	OnMaximumBulletChanged.Broadcast(MagazineCapacity);
}

void UGunComponent::OnRep_MagazineCapacity()
{
	OnMaximumBulletChanged.Broadcast(MagazineCapacity);
}

void UGunComponent::OnRep_RemainBullets()
{
	OnCurrentBulletChanged.Broadcast(RemainBullets);
}
