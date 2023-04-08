// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponComponent.h"

#include "Weapon/WeaponAssetData.h"
#include "Weapon/WeaponFire.h"
#include "Engine/DataTable.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"

UWeaponComponent::UWeaponComponent()
{
	bReplicateUsingRegisteredSubObjectList = true;
	bIsDataSetupRequested = false;
	UpgradeLevel = 0;
	MaximumUpgradeLevel = 4;

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
	if (UpgradeLevel > MaximumUpgradeLevel) return;
	OnUpgradeLevelChanged.Broadcast(++UpgradeLevel);
	GEngine->AddOnScreenDebugMessage(-1, 3, FColor::White, TEXT("Weapon Upgraded"));
	//TODO: 무기가 업그레이드 될 때 어떤 행동을 할 지 정의합니다.
}

void UWeaponComponent::UpgradeInitialize()
{
	UpgradeLevel = 0;
	OnUpgradeLevelChanged.Broadcast(UpgradeLevel);
}

void UWeaponComponent::SetupData()
{
	const auto Data = WeaponAssetDataTable->FindRow<FWeaponAssetData>(RequestedRowName, TEXT("WeaponComponent"));
	if (!Data) return;

	FireSubObject = CreateSingleSubObject<UWeaponFire>(Data->FireClass.LoadSynchronous(), Data->FireRowName);
	AbilitySubObject = CreateSingleSubObject<UWeaponAbility>(Data->AbilityClass.LoadSynchronous(),
	                                                         Data->AbilityRowName);
	ReloadSubObject = CreateSingleSubObject<UWeaponReload>(Data->ReloadClass.LoadSynchronous(), Data->ReloadRowName);

	WeaponUpgradeDataTable = Data->UpgradeTable.LoadSynchronous();

	if (!IsReplicatedSubObjectRegistered(FireSubObject))
		UE_LOG(LogTemp, Warning, TEXT("FireSubObject is NOT replicated"));
	if (!IsReplicatedSubObjectRegistered(AbilitySubObject))
		UE_LOG(LogTemp, Warning, TEXT("AbilitySubObject is NOT replicated"));
	if (!IsReplicatedSubObjectRegistered(ReloadSubObject))
		UE_LOG(LogTemp, Warning, TEXT("ReloadSubObject is NOT replicated"));
	if (WeaponUpgradeDataTable == nullptr)
		UE_LOG(LogTemp, Fatal, TEXT("UpgradeDataTable is not Found Please Check WeaponAssetDataTable"));
}

void UWeaponComponent::OnRep_UpgradeLevel()
{
	OnUpgradeLevelChanged.Broadcast(UpgradeLevel);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, FString::Printf(TEXT("OnRepUpgradeLevel")));
}

void UWeaponComponent::OnCharacterDead()
{
	FireSubObject->OnCharacterDead();
	AbilitySubObject->OnCharacterDead();
	ReloadSubObject->OnCharacterDead();
}

void UWeaponComponent::OnCharacterRespawn()
{
	FireSubObject->OnCharacterRespawn();
	AbilitySubObject->OnCharacterRespawn();
	ReloadSubObject->OnCharacterRespawn();
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, FireSubObject);
	DOREPLIFETIME(UWeaponComponent, AbilitySubObject);
	DOREPLIFETIME(UWeaponComponent, ReloadSubObject);
	DOREPLIFETIME(UWeaponComponent, UpgradeLevel);
}
