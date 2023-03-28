// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GunComponent.h"

#include "Weapon/GunAssetData.h"
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
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Reloading"));
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

void UGunComponent::SetupData()
{
	Super::SetupData();
	auto Data = WeaponAssetDataTable->FindRow<FGunAssetData>(RequestedRowName,TEXT("GunComponent"));
	if (!Data) return;

	RemainBullets = MagazineCapacity = Data->Magazine;
}
