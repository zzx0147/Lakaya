// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"

#include "WeaponAssetData.h"
#include "WeaponFire.h"
#include "Engine/DataTable.h"
#include "Net/UnrealNetwork.h"

UWeaponComponent::UWeaponComponent()
{
	bReplicateUsingRegisteredSubObjectList = true;
	bIsDataSetupRequested = false;

	static const ConstructorHelpers::FObjectFinder<UDataTable> DataFinder(
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/WeaponAssetDataTable'"));

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

void UWeaponComponent::SetupData()
{
	auto Data = WeaponAssetDataTable->FindRow<FWeaponAssetData>(RequestedRowName,TEXT("WeaponComponent"));
	if (!Data) return;

	FireSubObject = CreateSingleSubObject<UWeaponFire>(Data->FireClassPath.LoadSynchronous(), Data->FireRowName);
	AbilitySubObject = CreateSingleSubObject<UWeaponAbility>(Data->AbilityClassPath.LoadSynchronous(),
	                                                         Data->AbilityRowName);
	ReloadSubObject = CreateSingleSubObject<UWeaponReload>(Data->ReloadClassPath.LoadSynchronous(),
	                                                       Data->ReloadRowName);

	UE_LOG(LogTemp, Warning, TEXT("Fire Replication : %d"), IsReplicatedSubObjectRegistered(FireSubObject));
	UE_LOG(LogTemp, Warning, TEXT("Ability Replication : %d"), IsReplicatedSubObjectRegistered(AbilitySubObject));
	UE_LOG(LogTemp, Warning, TEXT("Reload Replication : %d"), IsReplicatedSubObjectRegistered(ReloadSubObject));
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, FireSubObject);
	DOREPLIFETIME(UWeaponComponent, AbilitySubObject);
	DOREPLIFETIME(UWeaponComponent, ReloadSubObject);
}
