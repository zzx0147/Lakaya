// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardReload.h"

#include "GunComponent.h"
#include "WeaponReloadData.h"
#include "Engine/DataTable.h"

UStandardReload::UStandardReload()
{
	static const ConstructorHelpers::FObjectFinder<UDataTable> TableFinder(
		TEXT("DataTable'/Game/Dev/Yongwoo/DataTables/WeaponReloadDataTable'"));

	if (TableFinder.Succeeded()) ReloadTable = TableFinder.Object;
}

void UStandardReload::SetupData_Implementation(const FName& RowName)
{
	Super::SetupData_Implementation(RowName);

	GunComponent = Cast<UGunComponent>(GetOuter());

	auto Data = ReloadTable->FindRow<FWeaponReloadData>(RowName,TEXT("StandardReload"));
	ReloadDelay = Data->ReloadDelay;
}

void UStandardReload::OnReloadStart()
{
	Super::OnReloadStart();

	if (GunComponent.IsValid()) GunComponent->SetFireEnabled(false);
	auto& TimerManager = GetWorld()->GetTimerManager();
	if (TimerManager.IsTimerActive(ReloadTimer)) return;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &UStandardReload::ReloadCallback, ReloadDelay);
}

void UStandardReload::OnReloadStartNotify()
{
	Super::OnReloadStartNotify();

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::White,TEXT("Reload"));
}

void UStandardReload::ReloadCallback()
{
	if (GunComponent.IsValid())
	{
		GunComponent->Reload();
		GunComponent->SetFireEnabled(true);
	}
}
