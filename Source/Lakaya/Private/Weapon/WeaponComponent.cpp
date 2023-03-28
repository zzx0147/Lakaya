// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponComponent.h"

#include "Weapon/WeaponAssetData.h"
#include "Weapon/WeaponFire.h"
#include "Engine/DataTable.h"
#include "Net/UnrealNetwork.h"

UWeaponComponent::UWeaponComponent()
{
	bReplicateUsingRegisteredSubObjectList = true;
	bIsDataSetupRequested = false;

	static const ConstructorHelpers::FObjectFinder<UDataTable> DataFinder(
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/DT_WeaponAssetDataTable'"));

	if (DataFinder.Succeeded()) WeaponAssetDataTable = DataFinder.Object;
}

void UWeaponComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
	if (bIsDataSetupRequested) SetupData();
}

void UWeaponComponent::RequestSetupData(const FName& RowName)
{
	RequestedRowName = RowName;
	bIsDataSetupRequested = true;
	if (IsReadyForReplication()) SetupData();
}

void UWeaponComponent::UpgradeWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White,TEXT("Upgraded"));
	//TODO: 무기가 업그레이드 될 때 어떤 행동을 할 지 정의합니다.
}

void UWeaponComponent::SetupData()
{
	auto Data = WeaponAssetDataTable->FindRow<FWeaponAssetData>(RequestedRowName,TEXT("WeaponComponent"));
	if (!Data) return;

	FireSubObject = CreateSingleSubObject<UWeaponFire>(Data->FireClass.LoadSynchronous(), Data->FireRowName);
	AbilitySubObject = CreateSingleSubObject<UWeaponAbility>(Data->AbilityClass.LoadSynchronous(),
	                                                         Data->AbilityRowName);
	ReloadSubObject = CreateSingleSubObject<UWeaponReload>(Data->ReloadClass.LoadSynchronous(), Data->ReloadRowName);

	if (!IsReplicatedSubObjectRegistered(FireSubObject))
		UE_LOG(LogTemp, Warning, TEXT("FireSubObject is NOT replicated"));
	if (!IsReplicatedSubObjectRegistered(AbilitySubObject))
		UE_LOG(LogTemp, Warning, TEXT("AbilitySubObject is NOT replicated"));
	if (!IsReplicatedSubObjectRegistered(ReloadSubObject))
		UE_LOG(LogTemp, Warning, TEXT("ReloadSubObject is NOT replicated"));
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, FireSubObject);
	DOREPLIFETIME(UWeaponComponent, AbilitySubObject);
	DOREPLIFETIME(UWeaponComponent, ReloadSubObject);
}
