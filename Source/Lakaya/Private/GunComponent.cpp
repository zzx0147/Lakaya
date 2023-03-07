// Fill out your copyright notice in the Description page of Project Settings.


#include "GunComponent.h"

#include "Net/UnrealNetwork.h"

void UGunComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGunComponent, MagazineCapacity);
	DOREPLIFETIME(UGunComponent, RemainBullets);
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

void UGunComponent::BeginPlay()
{
	Super::BeginPlay();
	//TODO: 임시 코드이므로 데이터를 받아서 설정하도록 변경해야 합니다.
	RemainBullets = MagazineCapacity = 30;
}
