// Fill out your copyright notice in the Description page of Project Settings.


#include "GunComponent.h"

#include "GunAssetData.h"
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
